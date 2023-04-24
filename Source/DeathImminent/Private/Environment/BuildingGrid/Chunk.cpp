// Copyright MikeSMediaStudios™ 2023


#include "Chunk.h"
#include <chrono>
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "RealtimeMeshLibrary.h"
#include "Utilities/GameUtilities.h"

AChunk::AChunk()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("Realtime Mesh Component"));
	RealtimeMeshComponent->SetupAttachment(RootComponent);
	OverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	OverlapCollision->SetupAttachment(RootComponent);
	OverlapCollision->SetCollisionProfileName("Volume");
}

void AChunk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Conditions;
	Conditions.Condition = COND_InitialOnly;
	DOREPLIFETIME_WITH_PARAMS(AChunk, m_WidthInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(AChunk, m_HeightInBlocks, Conditions);
	DOREPLIFETIME_WITH_PARAMS(AChunk, m_BlockSize, Conditions);
}

void AChunk::SetInformation(int32 width, int32 height, int32 blockSize)
{
	m_WidthInBlocks = width;
	m_HeightInBlocks = height;
	m_BlockSize = blockSize;

	const float ExtentXY = (width * blockSize) / 2;
	const float ExtentZ = (height * blockSize) / 2;

	OverlapCollision->SetBoxExtent(FVector(ExtentXY, ExtentXY, ExtentZ), true);
}

void AChunk::FillChunkZeroed()
{
	m_Blocks.SetNum(GetArrayDesiredSize());
}

void AChunk::FillChunkHalf()
{
	for (int x = 0; x < m_WidthInBlocks; x++)
	{
		for (int y = 0; y < m_WidthInBlocks; y++)
		{
			for (int z = 0; z < (m_HeightInBlocks / 2); z++)
			{
				m_Blocks[ConvertToArrayIndex(FInt32Vector(x, y, z))] = UGameUtilities::PackInt16ToInt32(0, 1);
			}
		}
	}
}

void AChunk::TryHitBlock(const FVector& Location)
{
	if (!ExecuteFunctionIfIndexValid(_GetBlockIndexFromLocation(Location), [this](const int32& Index)
		{
			m_Blocks[Index] = 0;
		})) return;

	UpdateMeshes();
}

void AChunk::TryPlaceBlock(const FVector& Location)
{
	if (!ExecuteFunctionIfIndexValid(_GetBlockIndexFromLocation(Location), [this](const int32& Index)
		{
			if (m_Blocks[Index] == 0)
				m_Blocks[Index] = UGameUtilities::PackInt16ToInt32(0, 1);
		})) return;

	UpdateMeshes();
}

void AChunk::UpdateMeshes()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("AGridChunk::UpdateMeshes()"))

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now(), end;

	_ResetMesh();
	FRealtimeMeshSimpleMeshData MeshData;

	_FindAllShapes([this, &MeshData](const FBlockShape& Shape)
		{
			_GenerateBox(Shape, m_BlockSize, MeshData);
		});

	const FRealtimeMeshSectionGroupKey GroupKey = m_Mesh->CreateSectionGroupWithMesh(FRealtimeMeshLODKey(0), MeshData);
	const FRealtimeMeshSectionConfig Config(ERealtimeMeshSectionDrawType::Dynamic, 0);
	m_Mesh->CreateSectionInGroup(GroupKey, Config, FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()), true);
	m_Mesh->UpdateCollision(true);

	end = std::chrono::high_resolution_clock::now();

	auto Duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	UE_LOG(LogTemp, Error, TEXT("Time taken to rebuild: %lldms Current FPS: %f"), Duration.count(), 1.0f / (UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) - std::chrono::duration_cast<std::chrono::seconds>(end - start).count()));
}

void AChunk::_ResetMesh()
{
	m_Mesh = RealtimeMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();
}

void AChunk::_GenerateBox(const FBlockShape& Shape, const int32& BlockSize, FRealtimeMeshSimpleMeshData& MeshData)
{
	const FBlockDataForMeshGeneration FirstBlock = Shape.Blocks[0];
	const FBlockDataForMeshGeneration LastBlock = Shape.Blocks[Shape.Blocks.Num() - 1];

	const int32 HalfBlockSize = BlockSize / 2;

	const float XMin = FirstBlock.Center.X - HalfBlockSize;
	const float XMax = LastBlock.Center.X + HalfBlockSize;
	const float LengthX = FMath::Abs(XMax - XMin);
	const float HalfLengthX = LengthX / 2;

	const float YMin = FirstBlock.Center.Y - HalfBlockSize;
	const float YMax = LastBlock.Center.Y + HalfBlockSize;
	const float LengthY = FMath::Abs(YMax - YMin);
	const float HalfLengthY = LengthY / 2;

	const float ZMin = FirstBlock.Center.Z - HalfBlockSize;
	const float ZMax = LastBlock.Center.Z + HalfBlockSize;
	const float LengthZ = FMath::Abs(ZMax - ZMin);
	const float HalfLengthZ = LengthZ / 2;

	FVector Center(XMax - HalfLengthX, YMax - HalfLengthY, ZMax - HalfLengthZ);
	Center -= GetActorLocation();

	URealtimeMeshBlueprintFunctionLibrary::AppendBoxMesh(FVector(HalfLengthX, HalfLengthY, HalfLengthZ), FTransform(Center), MeshData);
}

void AChunk::_FindAllShapes(TFunctionRef<void(const FBlockShape&)> GenerateMeshCallback)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("AChunk::FindAllShapes()"))

		TArray<bool> VisitedBlocks;
	VisitedBlocks.SetNumZeroed(GetArrayDesiredSize());

	for (int x = 0; x < m_WidthInBlocks; x++)
	{
		for (int z = 0; z < m_HeightInBlocks; z++)
		{
			for (int y = 0; y < m_WidthInBlocks; y++)
			{
				FBlockDataForMeshGeneration CurrentBlockData;
				if (!_IsBlockValidForShape(VisitedBlocks, FIntVector(x, y, z), CurrentBlockData))
					continue;

				FBlockShape CurrentShape;
				CurrentShape.Blocks.Add(CurrentBlockData);
				VisitedBlocks[CurrentBlockData.Index] = true;
				FIntVector CurrentBlockLocation = FIntVector(x, y, z);

				int32 MaxY = y;
				TArray<FBlockDataForMeshGeneration> BlocksOnY = _CheckBlocksForShapeOnY(VisitedBlocks, CurrentBlockLocation, MaxY);
				CurrentShape.LengthY = (MaxY - y) + 1;

				if (!BlocksOnY.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnY);

				int32 MaxZ = z;
				TArray<FBlockDataForMeshGeneration> BlocksOnZ = _CheckBlocksForShapeOnZ(VisitedBlocks, CurrentBlockLocation, MaxY, MaxZ);
				CurrentShape.LengthZ = (MaxZ - z) + 1;

				if (!BlocksOnZ.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnZ);

				int32 MaxX = x;
				TArray<FBlockDataForMeshGeneration> BlocksOnX = _CheckBlocksForShapeOnX(VisitedBlocks, CurrentBlockLocation, MaxY, MaxZ, MaxX);
				CurrentShape.LengthX = (MaxX - x) + 1;

				if (!BlocksOnX.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnX);

				GenerateMeshCallback(CurrentShape);
			}
		}
	}
}

bool AChunk::_IsBlockValidForShape(TArray<bool>& VisitedBlocks, const FIntVector& Location,
	FBlockDataForMeshGeneration& OutBlockData)
{
	const int32 BlockIndex = ConvertToArrayIndex(Location);

	if (VisitedBlocks[BlockIndex])
		return false;

	if (m_Blocks[BlockIndex] == 0)
		return false;

	OutBlockData = _FillBlockData(Location);

	return true;
}

TArray<FBlockDataForMeshGeneration> AChunk::_CheckBlocksForShapeOnY(TArray<bool>& VisitedBlocks,
	const FIntVector& Location, int32& OutMaxY)
{
	TArray<FBlockDataForMeshGeneration> Blocks;

	// Invalid index
	if (Location.Y > m_WidthInBlocks - 1)
		return Blocks;

	for (int y = Location.Y; y < m_WidthInBlocks; y++)
	{
		if (FIntVector(Location.X, y, Location.Z) == Location)
			continue;

		FBlockDataForMeshGeneration BlockData;
		if (!_IsBlockValidForShape(VisitedBlocks, FIntVector(Location.X, y, Location.Z), BlockData))
			return Blocks;

		Blocks.Add(BlockData);
		VisitedBlocks[BlockData.Index] = true;
		OutMaxY = y;
	}

	return Blocks;
}

TArray<FBlockDataForMeshGeneration> AChunk::_CheckBlocksForShapeOnZ(TArray<bool>& VisitedBlocks,
	const FIntVector& Location, const int32& MaxY, int32& OutMaxZ)
{
	TArray<FBlockDataForMeshGeneration> Blocks;

	int32 MaxZ = m_HeightInBlocks - 1;

	// Invalid index
	if (Location.Z > MaxZ)
		return Blocks;

	// Get the lowest average index for Z to see if we can combine
	for (int y = Location.Y; y <= MaxY; y++)
	{
		for (int z = Location.Z; z < m_HeightInBlocks; z++)
		{
			if ((y >= Location.Y && y <= MaxY) && z == Location.Z)
				continue;

			FBlockDataForMeshGeneration BlockData;
			if (!_IsBlockValidForShape(VisitedBlocks, FIntVector(Location.X, y, z), BlockData))
			{
				if (z == Location.Z + 1)
					return Blocks;

				if (z - 1 < MaxZ)
					MaxZ = z - 1;

				break;
			}
		}
	}

	OutMaxZ = MaxZ;

	// Add all blocks from the given span
	for (int y = Location.Y; y <= MaxY; y++)
	{
		for (int z = Location.Z; z <= MaxZ; z++)
		{
			FBlockDataForMeshGeneration BlockData = _FillBlockData(FIntVector(Location.X, y, z));
			Blocks.Add(BlockData);
			VisitedBlocks[BlockData.Index] = true;
		}
	}

	return Blocks;
}

TArray<FBlockDataForMeshGeneration> AChunk::_CheckBlocksForShapeOnX(TArray<bool>& VisitedBlocks,
	const FIntVector& Location, const int32& MaxY, const int32& MaxZ, int32& OutMaxX)
{
	TArray<FBlockDataForMeshGeneration> Blocks;

	int32 MaxX = m_WidthInBlocks - 1;

	// Invalid index
	if (Location.X > MaxX)
		return Blocks;

	// fix checks for X axis
	for (int y = Location.Y; y <= MaxY; y++)
	{
		for (int z = Location.Z; z <= MaxZ; z++)
		{
			for (int x = Location.X; x < m_WidthInBlocks; x++)
			{
				// Skip initial block as we have already added that one.
				if (x == Location.X && ((y >= Location.Y && y <= MaxY) && (z >= Location.Z && z <= MaxZ)))
					continue;

				FBlockDataForMeshGeneration BlockData;
				if (!_IsBlockValidForShape(VisitedBlocks, FIntVector(x, y, z), BlockData))
				{
					if (x == Location.X + 1)
						return Blocks;

					if (x - 1 < MaxX)
						MaxX = x - 1;
					break;
				}
			}
		}
	}

	OutMaxX = MaxX;

	for (int y = Location.Y; y <= MaxY; y++)
	{
		for (int z = Location.Z; z <= MaxZ; z++)
		{
			for (int x = Location.X; x <= MaxX; x++)
			{
				FBlockDataForMeshGeneration BlockData = _FillBlockData(FIntVector(x, y, z));
				Blocks.Add(BlockData);
				VisitedBlocks[BlockData.Index] = true;
			}
		}
	}

	return Blocks;
}

FBlockDataForMeshGeneration AChunk::_FillBlockData(const FIntVector& Location)
{
	const int32 Index = ConvertToArrayIndex(Location);
	FBlockDataForMeshGeneration BlockData;
	const FVector BlockLocationLocal = ((FVector(Location) * (m_BlockSize)) - (OverlapCollision->GetUnscaledBoxExtent()) + m_BlockSize / 2);

	UGameUtilities::UnpackInt32ToInt16(m_Blocks[Index], BlockData.Block.Source, BlockData.Block.ID);

	BlockData.Index = Index;
	BlockData.RelativeLocation = Location;
	BlockData.Center = ConvertToWorldSpaceFVector(BlockLocationLocal);

	return BlockData;
}
