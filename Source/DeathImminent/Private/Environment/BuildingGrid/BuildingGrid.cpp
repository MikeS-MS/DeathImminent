// Copyright MikeSMediaStudios™ 2023


#include "Environment/BuildingGrid/BuildingGrid.h"
#include "BuildingGridSubsystem.h"
#include "Environment/BuildingGrid/GridUtilities.h"
#include "Environment/BuildingGrid/ChunkStructs.h"
#include "Net/UnrealNetwork.h"
#include "SimplexNoise/Public/SimplexNoiseBPLibrary.h"

// Sets default values
ABuildingGrid::ABuildingGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	m_ChunkUpdateQueueComponent = CreateDefaultSubobject<UChunkUpdateQueueComponent>(TEXT("ChunkUpdateComponent"));
	m_GridBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Grid Bounds"));
	m_GridBounds->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	bReplicates = true;
}

void ABuildingGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	m_GridBounds->SetCollisionProfileName("Volume");
	__RecalculateSize();
}

// Called every frame
void ABuildingGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuildingGrid::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABuildingGrid, m_GridData);
	DOREPLIFETIME(ABuildingGrid, m__Chunks);
}

void ABuildingGrid::UpdateAllChunks()
{
	for (AChunk* Chunk : m__Chunks)
	{
		Chunk->_SetShouldUpdate();
	}
}

void ABuildingGrid::FillBlocks(int32 FromX, int32 Width, int32 FromY, int32 Depth, int32 FromZ, int32 Height, const FBaseID& BlockID)
{
	const FIntVector Size = m_GridData.SizeInBlocks;
	FromX = FMath::Clamp(FromX, 0, Size.X - 1);
	FromY = FMath::Clamp(FromY, 0, Size.Y - 1);
	FromZ = FMath::Clamp(FromZ, 0, Size.Z - 1);

	Width = Width < 0 ? Size.X : Width;
	Depth = Depth < 0 ? Size.Y : Depth;
	Height = Height < 0 ? Size.Z : Height;

	Width = FMath::Clamp(Width, 0, Size.X - FromX);
	Depth = FMath::Clamp(Depth, 0, Size.Y - FromY);
	Height = FMath::Clamp(Height, 0, Size.Z- FromZ);

	TArray<AChunk*> ModifiedChunks;

	for (int x = FromX; x < Width; x++)
	{
		for (int y = FromY; y < Depth; y++)
		{
			for (int z = FromZ; z < Height; z++)
			{
				//AChunk* Chunk = nullptr;
				__SetBlockAtGridLocation(FIntVector(x, y, z), BlockID, 1.0);
				//ModifiedChunks.AddUnique(Chunk);
			}
		}
	}

	//for (const auto& chunk : ModifiedChunks)
	//{
	//	chunk->_UpdateMesh();
	//}

}

ABuildingGrid* ABuildingGrid::GetInstance()
{
	return UBuildingGridSubsystem::GetInstance();
}

void ABuildingGrid::BeginPlay()
{
	Super::BeginPlay();

	USimplexNoiseBPLibrary::setNoiseSeed(Seed);
	__RecalculateSize();

	const FIntVector SizeInBlocks = m_GridData.SizeInBlocks;
	const FIntVector ChunksAmount = m_GridData.ChunksAmount;
	m__Chunks.SetNum(UGridUtilities::GetArrayDesiredSize3D(ChunksAmount.X, ChunksAmount.Y, ChunksAmount.Z));
	__SpawnChunks();

	switch (FillTestMethod)
	{
	case EFillTest::FT_Terrain:
	{
		__FillTestFromNoise();
		break;
	}
	case EFillTest::FT_Caves:
	{
		__FillCaves();
		break;
	}
	case EFillTest::FT_Plane:
	{
		FillBlocks(0, -1, 0, -1, 0, SizeInBlocks.Z / 2, FBaseID(1));
		break;
	}
	default:
		break;
	}

	UpdateAllChunks();
}

FSetBlockOperationResult ABuildingGrid::__SetBlockAtWorldLocation(const FVector& WorldLocation, 
																  const FBaseID& BlockID, 
																  const double Fullness, 
																  const bool Increments, 
																  const bool Force, 
																  const bool ShouldUpdate)
{
	if (!HasAuthority())
		return FSetBlockOperationResult();

	return __SetBlockAtGridLocation(ConvertWorldPosToGridPos(WorldLocation), BlockID, Fullness, Increments, Force, ShouldUpdate);
}

FSetBlockOperationResult ABuildingGrid::__SetBlockAtGridLocation(const FIntVector& GridLocation, 
																 const FBaseID& BlockID, 
																 const double Fullness, 
																 const bool Increments, 
																 const bool Force, 
																 const bool ShouldUpdate)
{
	if (!HasAuthority())
		return FSetBlockOperationResult();

	if (!IsValid(BlockID))
		return FSetBlockOperationResult();

	int32 ChunkIndexName = -1;
	if (!IsLocationInsideGridLocationAndChunkExists(GridLocation, ChunkIndexName))
		return FSetBlockOperationResult();

	AChunk* Chunk = m__Chunks[ChunkIndexName];

	if (!Chunk)
		return FSetBlockOperationResult();

	return Chunk->__SetBlockAtGridLocation(GridLocation, BlockID, Fullness, true, Increments, Force, ShouldUpdate);
}

AChunk* ABuildingGrid::_GetChunkAtGridLocation(const FIntVector& GridLocation)
{
	int32 Index;
	if (!IsLocationInsideGridLocationAndChunkExists(GridLocation, Index))
		return nullptr;

	return m__Chunks[Index];
}

AChunk* ABuildingGrid::_GetChunkChunkArrayLocation(const FIntVector& ArrayLocation)
{
	const FIntVector ChunksAmount = m_GridData.ChunksAmount;
	const int32 Index = UGridUtilities::Convert3DPosTo1DArrayIndex(ArrayLocation, ChunksAmount.X, ChunksAmount.Y);

	if (!m__Chunks.IsValidIndex(Index))
		return nullptr;

	return m__Chunks[Index];
}

FBlock ABuildingGrid::_GetBlockAtGridLocation(const FIntVector& GridLocation, bool& Result)
{
	const AChunk* Chunk = _GetChunkAtGridLocation(GridLocation);
	Result = false;

	if (!Chunk)
		return FBlock::Air;

	return Chunk->_GetBlockAtGridLocation(GridLocation, Result);
}

void ABuildingGrid::__SpawnChunks()
{
	UWorld* World = GetWorld();

	if (!World)
		return;

	const FVector SizeInUnits = m_GridData.SizeInUnits;
	const FVector ChunksSizeInUnits = m_GridData.ChunkSizeInUnits;
	const FIntVector ChunksAmount = m_GridData.ChunksAmount;
	const FIntVector ChunksSizeInBlocks = m_GridData.ChunkSizeInBlocks;

	const double StartX = ((SizeInUnits.X / 2.0) * -1.0) + (ChunksSizeInUnits.X / 2.0);
	const double StartY = ((SizeInUnits.Y / 2.0) * -1.0) + (ChunksSizeInUnits.Y / 2.0);
	const double StartZ = ((SizeInUnits.Z / 2.0) * -1.0) + (ChunksSizeInUnits.Z / 2.0);

	for (int x = 0; x < ChunksAmount.X; x++)
	{
		for (int y = 0; y < ChunksAmount.Y; y++)
		{
			for (int z = 0; z < ChunksAmount.Z; z++)
			{
				const double ChunkCenterX = StartX + (x * ChunksSizeInUnits.X);
				const double ChunkCenterY = StartY + (y * ChunksSizeInUnits.Y);
				const double ChunkCenterZ = StartZ + (z * ChunksSizeInUnits.Z);
				const int32 ChunkIndex = UGridUtilities::Convert3DPosTo1DArrayIndex(FIntVector(x, y, z), ChunksAmount.X, ChunksAmount.Y);

				AChunk* Chunk = World->SpawnActor<AChunk>(FVector(ChunkCenterX, ChunkCenterY, ChunkCenterZ), FRotator(), FActorSpawnParameters());
				Chunk->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));

				FChunkLocationData ChunkLocationData;
				ChunkLocationData.IndexInGrid = ChunkIndex;
				ChunkLocationData.PositionInGrid = FIntVector(x, y, z);
				ChunkLocationData.StartGridLocation = FIntVector(x * ChunksSizeInBlocks.X, y * ChunksSizeInBlocks.Y, z * ChunksSizeInBlocks.Z);
				ChunkLocationData.EndGridLocation = ChunkLocationData.StartGridLocation + (ChunksSizeInBlocks - FIntVector(1));
				Chunk->_SetInformation(this, ChunkLocationData);
				Chunk->InitializeBlockArrayWithAir();
				m__Chunks[ChunkIndex] = Chunk;
			}
		}
	}
}

void ABuildingGrid::__RecalculateSize()
{
	m_GridData.SizeInUnits = FVector(m_GridData.SizeInBlocks * FIntVector(m_GridData.BlockSize));
	m_GridData.ChunkSizeInUnits = FVector(m_GridData.ChunkSizeInBlocks * m_GridData.BlockSize);

	m_GridData.ChunksAmount.X = m_GridData.SizeInBlocks.X / m_GridData.ChunkSizeInBlocks.X;
	m_GridData.ChunksAmount.Y = m_GridData.SizeInBlocks.Y / m_GridData.ChunkSizeInBlocks.Y;
	m_GridData.ChunksAmount.Z = m_GridData.SizeInBlocks.Z / m_GridData.ChunkSizeInBlocks.Z;

	m_GridBounds->SetBoxExtent(m_GridData.SizeInUnits / 2);
}

void ABuildingGrid::__FillTestFromNoise()
{
	const FIntVector SizeInBlocks = m_GridData.SizeInBlocks;

	for (int x = 0; x < SizeInBlocks.X; x++)
	{
		for (int y = 0; y < SizeInBlocks.Y; y++)
		{
			for (int z = 0; z < Floor; z++)
			{
				FSetBlockOperationResult SetBlockOperationResult = __SetBlockAtGridLocation(FIntVector(x, y, z), FBaseID(1), 0.5, false, true, false);
			}

			const float Value = USimplexNoiseBPLibrary::GetSimplexNoise2D_EX(x, y, Lucanarity, Persistence, Octaves, Zoom, true);
			const int Height = (Value * static_cast<double>(Ceiling)) + Floor;
			for (int z = Floor; z < Height; z++)
			{
				FSetBlockOperationResult SetBlockOperationResult = __SetBlockAtGridLocation(FIntVector(x, y, z), FBaseID(1), 0.5, false, true, false);
			}
		}
	}
}

void ABuildingGrid::__FillCaves()
{
	const FIntVector SizeInBlocks = m_GridData.SizeInBlocks;

	for (int x = 0; x < SizeInBlocks.X; x++)
	{
		for (int y = 0; y < SizeInBlocks.Y; y++)
		{
			for (int z = 0; z < SizeInBlocks.Z; z++)
			{
				const float Value = USimplexNoiseBPLibrary::GetSimplexNoise3D_EX(x, y, z, Lucanarity, Persistence, Octaves, Zoom, true);

				if (Value > .4f)
					__SetBlockAtGridLocation(FIntVector(x, y, z), FBaseID(1),1.0);
			}
		}
	}
}
