// Copyright MikeSMediaStudios™ 2023


#include "Environment/BuildingGrid/BuildingGrid.h"
#include "Environment/BuildingGrid/GridUtilities.h"
#include "Net/UnrealNetwork.h"

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
	SetSizes();
}

void ABuildingGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	m_GridBounds->SetCollisionProfileName("Volume");
	RecalculateSize();
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
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_BlockSize, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_ChunkWidthInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_ChunkHeightInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_WidthInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_HeightInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_ChunkWidthInUnits, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_ChunkHeightInUnits, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_WidthInUnits, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_HeightInUnits, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_ChunksAmountXY, Conditions);
	DOREPLIFETIME_WITH_PARAMS(ABuildingGrid, m_ChunksAmountZ, Conditions);
}

void ABuildingGrid::UpdateChunks()
{
	for (AChunk* Chunk : m_Chunks)
	{
		Chunk->UpdateMesh();
	}
}

void ABuildingGrid::TryHitBlock(const FVector& Location)
{
	ExecuteFunctionIfInside(Location, [this](const int32& Index, const FVector& Location)
		{
			AChunk* Chunk = m_Chunks[Index];

			if (!Chunk)
				return;

			Chunk->TryHitBlock(Location);
		});
}

void ABuildingGrid::TryPlaceBlock(const FVector& Location)
{
	ExecuteFunctionIfInside(Location, [this](const int32& Index, const FVector& Location)
		{
			AChunk* Chunk = m_Chunks[Index];

			if (!Chunk)
				return;

			Chunk->TryPlaceBlock(Location, FBlockID(0, 1), true);
		});
}

FBlockID ABuildingGrid::GetBlockAtWorldLocation(const FVector& Location)
{
	if (!IsLocationInsideGridInUnits(Location))
		return FBlockID::Invalid();

	const AChunk* Chunk = m_Chunks[_GetArrayIndexFromLocation(Location)];
	return Chunk->GetBlockAtWorldLocation(Location);
}

FBlockID ABuildingGrid::GetBlockAtGridLocation(const FIntVector& Location)
{
	return GetBlockAtGridLocationXYZ(Location.X, Location.Y, Location.Z);
}

FBlockID ABuildingGrid::GetBlockAtGridLocationXYZ(const int& x, const int& y, const int& z)
{
	if (!IsLocationInsideGridInBlocks(x, y ,z))
		return FBlockID::Air();

	const FIntVector ContainingChunkIndexVector = UGridUtilities::ConvertToArrayIndexVector(FVector(x, y ,z), m_ChunkWidthInBlocks, m_ChunkHeightInBlocks);
	const int32 ContainingChunkIndex = UGridUtilities::ConvertToArrayIndex(ContainingChunkIndexVector, m_ChunksAmountXY);

	const AChunk* Chunk = m_Chunks[ContainingChunkIndex];
	return Chunk->GetBlockAtGridLocation(x, y ,z, true);
}

void ABuildingGrid::FillBlocks(int32 FromX, int32 Width, int32 FromY, int32 Depth, int32 FromZ, int32 Height, const FBlockID& BlockID)
{
	FromX = FMath::Clamp(FromX, 0, m_WidthInBlocks - 1);
	FromY = FMath::Clamp(FromY, 0, m_WidthInBlocks - 1);
	FromZ = FMath::Clamp(FromZ, 0, m_HeightInBlocks - 1);

	Width = Width == -1 ? m_WidthInBlocks : Width;
	Depth = Depth == -1 ? m_WidthInBlocks : Depth;
	Height = Height == -1 ? m_HeightInBlocks : Height;

	Width = FMath::Clamp(Width, 0, m_WidthInBlocks - FromX);
	Depth = FMath::Clamp(Depth, 0, m_WidthInBlocks - FromY);
	Height = FMath::Clamp(Height, 0, m_HeightInBlocks - FromZ);

	const float& HalfBlockSize = static_cast<float>(m_BlockSize) / 2.f;
	const FVector Extent = m_GridBounds->GetUnscaledBoxExtent();

	TArray<AChunk*> ModifiedChunks;
	AChunk* Chunk = nullptr;
	const float StartXY = -Extent.X + HalfBlockSize;
	const float StartZ = -Extent.Z + HalfBlockSize;

	for (int x = FromX; x < Width; x++)
	{
		for (int y = FromY; y < Depth; y++)
		{
			for (int z = FromZ; z < Height; z++)
			{
				ExecuteFunctionIfInside(FVector(StartXY + (x * m_BlockSize), StartXY + (y * m_BlockSize), StartZ + (z * m_BlockSize)), [x, y, z, this, &Chunk, &ModifiedChunks, &BlockID](const int32& Index, const FVector& Location)
					{
						Chunk = m_Chunks[Index];
						if (!Chunk)
							return;

						ModifiedChunks.AddUnique(Chunk);
						Chunk->TryPlaceBlock(Location, BlockID, false);
					});
			}
		}
	}

	for (const auto& chunk : ModifiedChunks)
	{
		chunk->UpdateMesh();
	}

}

void ABuildingGrid::BeginPlay()
{
	Super::BeginPlay();
	RecalculateSize();
	m_Chunks.SetNum(UGridUtilities::GetArrayDesiredSize(m_ChunksAmountXY, m_ChunksAmountZ));
	SpawnChunks();
	FillBlocks(0, -1, 0, -1, 0, m_HeightInBlocks / 2, FBlockID(0, 1));
}

void ABuildingGrid::SetSizes(const int32& BlockSize, const int32& ChunkWidthInBlocks,
	const int32& ChunkHeightInBlocks, const int32& WidthInBlocks, const int32& HeightInBlocks)
{
	m_BlockSize = BlockSize;
	m_ChunkWidthInBlocks = ChunkWidthInBlocks;
	m_ChunkHeightInBlocks = ChunkHeightInBlocks;
	m_WidthInBlocks = WidthInBlocks;
	m_HeightInBlocks = HeightInBlocks;

	RecalculateSize();
}

void ABuildingGrid::RecalculateSize()
{
	m_WidthInUnits = m_WidthInBlocks * m_BlockSize;
	m_HeightInUnits = m_HeightInBlocks * m_BlockSize;
	m_ChunkWidthInUnits = m_ChunkWidthInBlocks * m_BlockSize;
	m_ChunkHeightInUnits = m_ChunkHeightInBlocks * m_BlockSize;
	m_ChunksAmountXY = m_WidthInBlocks / m_ChunkWidthInBlocks;
	m_ChunksAmountZ = m_HeightInBlocks / m_ChunkHeightInBlocks;

	const float XYExtent = static_cast<float>(m_WidthInUnits) / 2.f;
	const float ZExtent = static_cast<float>(m_HeightInUnits) / 2.f;
	m_GridBounds->SetBoxExtent(FVector(XYExtent, XYExtent, ZExtent));
}

void ABuildingGrid::SpawnChunks()
{
	UWorld* World = GetWorld();

	if (!World)
		return;

	const float StartXY = ((static_cast<float>(m_WidthInUnits) / 2.f) * -1.f) + (static_cast<float>(m_ChunkWidthInUnits) / 2.f);
	const float StartZ = ((static_cast<float>(m_HeightInUnits) / 2.f) * -1.f) + (static_cast<float>(m_ChunkHeightInUnits) / 2.f);

	int StartXIndex = 0;
	int StartYIndex = 0;
	int StartZIndex = 0;

	for (int x = 0; x < m_ChunksAmountXY; x++)
	{
		for (int y = 0; y < m_ChunksAmountXY; y++)
		{
			for (int z = 0; z < m_ChunksAmountZ; z++)
			{
				const float ChunkCenterX = StartXY + (x * m_ChunkWidthInUnits);
				const float ChunkCenterY = StartXY + (y * m_ChunkWidthInUnits);
				const float ChunkCenterZ = StartZ + (z * m_ChunkHeightInUnits);
				const int32 ChunkIndex = UGridUtilities::ConvertToArrayIndex(FIntVector(x, y, z), m_ChunksAmountXY);

				AChunk* Chunk = World->SpawnActor<AChunk>(FVector(ChunkCenterX, ChunkCenterY, ChunkCenterZ), FRotator(), FActorSpawnParameters());
				Chunk->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
				Chunk->SetInformation(this, FIntVector(x, y ,z), ChunkIndex, x * m_ChunkWidthInBlocks, y * m_ChunkWidthInBlocks, z * m_ChunkHeightInBlocks);
				Chunk->FillChunkZeroed();
				m_Chunks[ChunkIndex] = Chunk;
			}
		}
	}
}