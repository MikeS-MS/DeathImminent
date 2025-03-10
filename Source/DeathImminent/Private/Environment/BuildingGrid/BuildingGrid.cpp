// Copyright MikeSMediaStudios™ 2023

#include "Environment/BuildingGrid/BuildingGrid.h"
#include "Environment/BuildingGrid/GridUtilities.h"
#include "Net/UnrealNetwork.h"
#include "SimplexNoise/Public/SimplexNoiseBPLibrary.h"

// Sets default values
ABuildingGrid::ABuildingGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	m_GridBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Grid Bounds"));
	m_GridBounds->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	bReplicates = true;
	__SetSizes();
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

	FDoRepLifetimeParams Conditions;
	Conditions.Condition = COND_InitialOnly;
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__BlockSize, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__ChunkWidthInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__ChunkHeightInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__WidthInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__HeightInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__ChunkWidthInUnits, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__ChunkHeightInUnits, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__WidthInUnits, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__HeightInUnits, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__ChunksAmountXY, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m__ChunksAmountZ, Conditions);
}

void ABuildingGrid::UpdateAllChunks()
{
	for (AChunk* Chunk : m__Chunks)
	{
		Chunk->UpdateMesh();
	}
}

void ABuildingGrid::TryHitBlock(const FVector& Location)
{
	__ExecuteFunctionIfInside(Location, [this](const int32& Index, const FVector& Location)
		{
			AChunk* Chunk = m__Chunks[Index];

			if (!Chunk)
				return;

			Chunk->TryHitBlock(Location);
		});
}

void ABuildingGrid::TryPlaceBlock(const FVector& Location, const bool UpdateMesh)
{
	__ExecuteFunctionIfInside(Location, [this, &UpdateMesh](const int32& Index, const FVector& Location)
		{
			AChunk* Chunk = m__Chunks[Index];

			if (!Chunk)
				return;

			Chunk->TryPlaceBlock(Location, FBlockID(0, 1), UpdateMesh);
		});
}

void ABuildingGrid::FillBlocks(int32 FromX, int32 Width, int32 FromY, int32 Depth, int32 FromZ, int32 Height, const FBlockID& BlockID)
{
	FromX = FMath::Clamp(FromX, 0, m__WidthInBlocks - 1);
	FromY = FMath::Clamp(FromY, 0, m__WidthInBlocks - 1);
	FromZ = FMath::Clamp(FromZ, 0, m__HeightInBlocks - 1);

	Width = Width == -1 ? m__WidthInBlocks : Width;
	Depth = Depth == -1 ? m__WidthInBlocks : Depth;
	Height = Height == -1 ? m__HeightInBlocks : Height;

	Width = FMath::Clamp(Width, 0, m__WidthInBlocks - FromX);
	Depth = FMath::Clamp(Depth, 0, m__WidthInBlocks - FromY);
	Height = FMath::Clamp(Height, 0, m__HeightInBlocks - FromZ);

	TArray<AChunk*> ModifiedChunks;

	for (int x = FromX; x < Width; x++)
	{
		for (int y = FromY; y < Depth; y++)
		{
			for (int z = FromZ; z < Height; z++)
			{
				AChunk* Chunk = nullptr;
				SetBlockAtGridLocation_GetChunkRef(BlockID, x, y, z, Chunk, false);
				ModifiedChunks.AddUnique(Chunk);
			}
		}
	}

	for (const auto& chunk : ModifiedChunks)
	{
		chunk->UpdateMesh();
	}

}

FBlockID ABuildingGrid::GetBlockAtWorldLocation(const FVector& Location)
{
	if (!IsLocationInsideGridInUnits(Location))
		return FBlockID::Invalid;

	const AChunk* Chunk = m__Chunks[_GetArrayIndexFromWorldLocation(Location)];
	return Chunk->GetBlockAtWorldLocation(Location);
}

FBlockID ABuildingGrid::GetBlockAtGridLocation(const int& x, const int& y, const int& z)
{
	if (!IsLocationInsideGridInBlocks(x, y ,z))
		return FBlockID::Air;

	const AChunk* Chunk = m__Chunks[_GetArrayIndexFromGridLocation(x, y, z)];
	return Chunk->GetBlockAtGridLocation(x, y ,z, true);
}

void ABuildingGrid::BeginPlay()
{
	Super::BeginPlay();
	USimplexNoiseBPLibrary::setNoiseSeed(Seed);
	__RecalculateSize();
	m__Chunks.SetNum(UGridUtilities::GetArrayDesiredSize(m__ChunksAmountXY, m__ChunksAmountZ));
	__SpawnChunks();
	//FillBlocks(0, -1, 0, -1, 0, m_HeightInBlocks / 2, FBlockID(0, 1));

	if (Caves)
		__FillCaves();
	else
		__FillTestFromNoise();

	UpdateAllChunks();
}

void ABuildingGrid::__FillTestFromNoise()
{
	for (int x = 0; x < m__WidthInBlocks; x++)
	{
		for (int y = 0; y < m__WidthInBlocks; y++)
		{
			for (int z = 0; z < Floor; z++)
			{
				SetBlockAtGridLocation(FBlockID(0, 1), x, y, z, false);
			}
		}
	}
	for (int x = 0; x < m__WidthInBlocks; x++)
	{
		for (int y = 0; y < m__WidthInBlocks; y++)
		{
			const float Value = USimplexNoiseBPLibrary::GetSimplexNoise2D_EX(x, y, Lucanarity, Persistence, Octaves, Zoom, true);
			const int Height = (Value * static_cast<float>(Ceiling)) + Floor;
			for (int z = Floor; z < Height; z++)
			{
				SetBlockAtGridLocation(FBlockID(0, 1), x, y, z, false);
			}
		}
	}
}

void ABuildingGrid::__FillCaves()
{
	for (int x = 0; x < m__WidthInBlocks; x++)
	{
		for (int y = 0; y < m__WidthInBlocks; y++)
		{
			for (int z = 0; z < m__HeightInBlocks; z++)
			{
				const float Value = USimplexNoiseBPLibrary::GetSimplexNoise3D_EX(x, y, z, Lucanarity, Persistence, Octaves, Zoom, true);

				if (Value > .4f)
					SetBlockAtGridLocation(FBlockID(0, 1), x, y, z, false);
			}
		}
	}
}

void ABuildingGrid::__SetSizes(const int32& BlockSize, const int32& ChunkWidthInBlocks,
                              const int32& ChunkHeightInBlocks, const int32& WidthInBlocks, const int32& HeightInBlocks)
{
	m__BlockSize = BlockSize;
	m__ChunkWidthInBlocks = ChunkWidthInBlocks;
	m__ChunkHeightInBlocks = ChunkHeightInBlocks;
	m__WidthInBlocks = WidthInBlocks;
	m__HeightInBlocks = HeightInBlocks;

	__RecalculateSize();
}

void ABuildingGrid::__RecalculateSize()
{
	m__WidthInUnits = m__WidthInBlocks * m__BlockSize;
	m__HeightInUnits = m__HeightInBlocks * m__BlockSize;
	m__ChunkWidthInUnits = m__ChunkWidthInBlocks * m__BlockSize;
	m__ChunkHeightInUnits = m__ChunkHeightInBlocks * m__BlockSize;
	m__ChunksAmountXY = m__WidthInBlocks / m__ChunkWidthInBlocks;
	m__ChunksAmountZ = m__HeightInBlocks / m__ChunkHeightInBlocks;

	const float XYExtent = static_cast<float>(m__WidthInUnits) / 2.f;
	const float ZExtent = static_cast<float>(m__HeightInUnits) / 2.f;
	m_GridBounds->SetBoxExtent(FVector(XYExtent, XYExtent, ZExtent));
}

void ABuildingGrid::__SpawnChunks()
{
	UWorld* World = GetWorld();

	if (!World)
		return;

	const float StartXY = ((static_cast<float>(m__WidthInUnits) / 2.f) * -1.f) + (static_cast<float>(m__ChunkWidthInUnits) / 2.f);
	const float StartZ = ((static_cast<float>(m__HeightInUnits) / 2.f) * -1.f) + (static_cast<float>(m__ChunkHeightInUnits) / 2.f);

	int StartXIndex = 0;
	int StartYIndex = 0;
	int StartZIndex = 0;

	for (int x = 0; x < m__ChunksAmountXY; x++)
	{
		for (int y = 0; y < m__ChunksAmountXY; y++)
		{
			for (int z = 0; z < m__ChunksAmountZ; z++)
			{
				const float ChunkCenterX = StartXY + (x * m__ChunkWidthInUnits);
				const float ChunkCenterY = StartXY + (y * m__ChunkWidthInUnits);
				const float ChunkCenterZ = StartZ + (z * m__ChunkHeightInUnits);
				const int32 ChunkIndex = UGridUtilities::ConvertToArrayIndex(x, y, z, m__ChunksAmountXY);

				AChunk* Chunk = World->SpawnActor<AChunk>(FVector(ChunkCenterX, ChunkCenterY, ChunkCenterZ), FRotator(), FActorSpawnParameters());
				Chunk->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
				Chunk->SetInformation(this, FIntVector(x, y ,z), ChunkIndex, x * m__ChunkWidthInBlocks, y * m__ChunkWidthInBlocks, z * m__ChunkHeightInBlocks);
				Chunk->InitializeBlockArrayWithAir();
				m__Chunks[ChunkIndex] = Chunk;
			}
		}
	}
}