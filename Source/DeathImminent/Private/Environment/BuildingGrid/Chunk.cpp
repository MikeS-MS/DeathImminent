// Copyright MikeSMediaStudios™ 2023


#include "Environment/BuildingGrid/Chunk.h"
//#include <chrono>
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Environment/BuildingGrid/BuildingGrid.h"
#include "Environment/BuildingGrid/GridUtilities.h"
#include "Utilities/GameUtilities.h"


AChunk::AChunk()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	m_RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("Realtime Mesh Component"));
	m_RealtimeMeshComponent->SetupAttachment(RootComponent);
	m_OverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	m_OverlapCollision->SetupAttachment(RootComponent);
	m_OverlapCollision->SetCollisionProfileName("Volume");

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> Material(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Temp/Materials/M_AlignedMaterial_Inst.M_AlignedMaterial_Inst'"));

	if (Material.Object != nullptr)
	{
		m_GridMaterial = Cast<UMaterialInstance>(Material.Object);
	}
}

void AChunk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Conditions;
	Conditions.Condition = COND_InitialOnly;
}

void AChunk::SetInformation(ABuildingGrid* ContainingGrid, const FIntVector& ChunkPosInGrid, const int32& ChunkIndexInGrid, const int32& StartX, const int32& StartY, const int32& StartZ)
{
	m_ContainingGrid = ContainingGrid;
	m_ChunkPosInGrid = ChunkPosInGrid;
	m_ChunkIndexInGrid = ChunkIndexInGrid;

	m_StartX = StartX;
	m_StartY = StartY;
	m_StartZ = StartZ;

	m_WidthInBlocks = ContainingGrid->GetChunkWidthInBlocks();
	m_HeightInBlocks = ContainingGrid->GetChunkHeightInBlocks();
	m_BlockSize = ContainingGrid->GetBlockSize();

	m_EndX = StartX + m_WidthInBlocks - 1;
	m_EndY = StartY + m_WidthInBlocks - 1;
	m_EndZ = StartZ + m_HeightInBlocks - 1;

	const float ExtentXY = (m_WidthInBlocks * m_BlockSize) / 2;
	const float ExtentZ = (m_HeightInBlocks * m_BlockSize) / 2;

	m_OverlapCollision->SetBoxExtent(FVector(ExtentXY, ExtentXY, ExtentZ), true);
}

void AChunk::FillChunkZeroed()
{
	m_Blocks.SetNum(UGridUtilities::GetArrayDesiredSize(m_WidthInBlocks, m_HeightInBlocks));
}

void AChunk::FillChunk(const int32& FillX, const int32& FillY, const int32& FillZ, const FBlockID& BlockID)
{
	const int32 FillXModified = FillX < 0 ? m_WidthInBlocks : FillX;
	const int32 FillYModified = FillY < 0 ? m_WidthInBlocks : FillY;
	const int32 FillZModified = FillZ < 0 ? m_HeightInBlocks : FillZ;

	for (int x = 0; x < FillXModified; x++)
	{
		for (int y = 0; y < FillYModified; y++)
		{
			for (int z = 0; z < FillZModified; z++)
			{
				m_Blocks[UGridUtilities::ConvertToArrayIndex(FInt32Vector(x, y, z), m_WidthInBlocks)] = UGridUtilities::PackInt16ToInt32(BlockID.Source, BlockID.ID);
			}
		}
	}
}

void AChunk::TryHitBlock(const FVector& Location)
{
	FVector LocalSpaceLocation;
	FVector ArrayUsableLocation;
	FIntVector GridLocation;

	if (!_ExecuteFunctionIfIndexValid(_GetBlockIndexFromLocation(Location, LocalSpaceLocation, ArrayUsableLocation, GridLocation), [this, &Location](const int32& Index)
		{
			if (m_Blocks[Index] == 0)
				return;

			m_Blocks[Index] = 0;
		})) return;

	UpdateMesh();
	_UpdateMeshSurroundingChunks(GridLocation.X, GridLocation.Y, GridLocation.Z);
}

bool AChunk::TryPlaceBlock(const FVector& Location, const FBlockID& BlockID, const bool& ShouldUpdateMeshes)
{
	FVector LocalSpaceLocation;
	FVector ArrayUsableLocation;
	FIntVector GridLocation;

	if (!_ExecuteFunctionIfIndexValid(_GetBlockIndexFromLocation(Location, LocalSpaceLocation, ArrayUsableLocation, GridLocation), [this, &BlockID, &GridLocation](const int32& Index)
		{
			if (m_Blocks[Index] != 0)
				return;

			m_Blocks[Index] = UGridUtilities::PackInt16ToInt32(BlockID.Source, BlockID.ID);
		})) return false;

	if (ShouldUpdateMeshes)
	{
		UpdateMesh();
		_UpdateMeshSurroundingChunks(GridLocation.X, GridLocation.Y, GridLocation.Z);
	}

	return true;
}

void AChunk::UpdateMesh()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("AGridChunk::UpdateMesh()"))

		//std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now(), end;

		_ResetMesh();
	FRealtimeMeshSimpleMeshData MeshData;

	if (m_ContainingGrid->m_GreedyMeshing)
	{
		_FindAllShapes([this, &MeshData](const FGreedyMeshShape& Shape)
			{
				_GenerateBox(Shape, m_BlockSize, MeshData);
			});
	}
	else
	{
		MarchBlocks(MeshData);
	}
	m_Mesh->SetupMaterialSlot(0, TEXT("Material"));
	const FRealtimeMeshSectionGroupKey GroupKey = m_Mesh->CreateSectionGroupWithMesh(FRealtimeMeshLODKey(0), MeshData);
	const FRealtimeMeshSectionConfig Config(ERealtimeMeshSectionDrawType::Dynamic, 0);
	m_Mesh->CreateSectionInGroup(GroupKey, Config, FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()), true);
	m_Mesh->UpdateCollision(true);
	m_RealtimeMeshComponent->SetMaterial(0, m_GridMaterial);

	//end = std::chrono::high_resolution_clock::now();

	//auto Duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//UE_LOG(LogTemp, Error, TEXT("Time taken to rebuild: %lldms Current FPS: %f"), Duration.count(), 1.0f / (UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) - std::chrono::duration_cast<std::chrono::seconds>(end - start).count()));
}

void AChunk::_ResetMesh()
{
	m_Mesh = m_RealtimeMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();
}

void AChunk::_UpdateMeshSurroundingChunks(const int& x, const int& y, const int& z)
{
	const int bIsXEdge = x == 0 ? -1 : x == m_WidthInBlocks - 1 ? 1 : 0;
	const int bIsYEdge = y == 0 ? -1 : y == m_WidthInBlocks - 1 ? 1 : 0;
	const int bIsZEdge = z == 0 ? -1 : z == m_WidthInBlocks - 1 ? 1 : 0;

	if (bIsXEdge != 0)
	{
		AChunk* LeftRightChunk = m_ContainingGrid->GetChunkAtArrayPosition(FIntVector(m_ChunkPosInGrid.X + bIsXEdge, m_ChunkPosInGrid.Y, m_ChunkPosInGrid.Z));
		if (IsValid(LeftRightChunk))
			LeftRightChunk->UpdateMesh();
	}

	if (bIsYEdge != 0)
	{
		AChunk* FrontBackChunk = m_ContainingGrid->GetChunkAtArrayPosition(FIntVector(m_ChunkPosInGrid.X, m_ChunkPosInGrid.Y + bIsYEdge, m_ChunkPosInGrid.Z));
		if (IsValid(FrontBackChunk))
			FrontBackChunk->UpdateMesh();
	}

	if (bIsZEdge != 0)
	{
		AChunk* BottomTopChunk = m_ContainingGrid->GetChunkAtArrayPosition(FIntVector(m_ChunkPosInGrid.X, m_ChunkPosInGrid.Y, m_ChunkPosInGrid.Z + bIsZEdge));
		if (IsValid(BottomTopChunk))
			BottomTopChunk->UpdateMesh();
	}

	if ((bIsXEdge != 0 && bIsYEdge != 0) && bIsZEdge != 0)
	{
		AChunk* CornerChunk = m_ContainingGrid->GetChunkAtArrayPosition(FIntVector(m_ChunkPosInGrid.X + bIsXEdge, m_ChunkPosInGrid.Y + bIsYEdge, m_ChunkPosInGrid.Z + bIsZEdge));
		if (IsValid(CornerChunk))
			CornerChunk->UpdateMesh();
	}

	if (bIsXEdge != 0 && bIsYEdge != 0)
	{
		AChunk* EdgeChunk = m_ContainingGrid->GetChunkAtArrayPosition(FIntVector(m_ChunkPosInGrid.X + bIsXEdge, m_ChunkPosInGrid.Y + bIsYEdge, m_ChunkPosInGrid.Z));
		if (IsValid(EdgeChunk))
			EdgeChunk->UpdateMesh();
	}

	if (bIsXEdge != 0 && bIsZEdge != 0)
	{
		AChunk* EdgeChunk = m_ContainingGrid->GetChunkAtArrayPosition(FIntVector(m_ChunkPosInGrid.X + bIsXEdge, m_ChunkPosInGrid.Y, m_ChunkPosInGrid.Z + bIsZEdge));
		if (IsValid(EdgeChunk))
			EdgeChunk->UpdateMesh();
	}

	if (bIsYEdge != 0 && bIsZEdge != 0)
	{
		AChunk* EdgeChunk = m_ContainingGrid->GetChunkAtArrayPosition(FIntVector(m_ChunkPosInGrid.X, m_ChunkPosInGrid.Y + bIsYEdge, m_ChunkPosInGrid.Z + bIsZEdge));
		if (IsValid(EdgeChunk))
			EdgeChunk->UpdateMesh();
	}

}

FBlockID AChunk::_GetBlockAtLocationOptimizedForLocal(const int& x, const int& y, const int& z,
	const bool& GlobalGridCoordinates) const
{
	if (IsLocationInsideChunkInBlocks(x, y, z, GlobalGridCoordinates))
		return GetBlockAtGridLocation(x, y, z, GlobalGridCoordinates);

	int newX = x;
	int newY = y;
	int newZ = z;
	ConvertGridLocation(newX, newY, newZ, false);

	return m_ContainingGrid->GetBlockAtGridLocationXYZ(newX, newY, newZ);
}

void AChunk::_FindAllShapes(TFunctionRef<void(const FGreedyMeshShape&)> GenerateMeshCallback)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("AChunk::FindAllShapes()"))

		TArray<bool> VisitedBlocks;
	VisitedBlocks.SetNum(UGridUtilities::GetArrayDesiredSize(m_WidthInBlocks, m_HeightInBlocks));

	for (int x = 0; x < m_WidthInBlocks; x++)
	{
		for (int z = 0; z < m_HeightInBlocks; z++)
		{
			for (int y = 0; y < m_WidthInBlocks; y++)
			{
				FBlockDataForGreedyMeshGeneration CurrentBlockData;
				if (!_IsBlockValidForShape(VisitedBlocks, FIntVector(x, y, z), CurrentBlockData))
					continue;

				FGreedyMeshShape CurrentShape;
				CurrentShape.Blocks.Add(CurrentBlockData);
				VisitedBlocks[CurrentBlockData.Index] = true;
				FIntVector CurrentBlockLocation = FIntVector(x, y, z);

				int32 MaxY = y;
				TArray<FBlockDataForGreedyMeshGeneration> BlocksOnY = _CheckBlocksForShapeOnY(VisitedBlocks, CurrentBlockLocation, MaxY);
				CurrentShape.LengthY = (MaxY - y) + 1;

				if (!BlocksOnY.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnY);

				int32 MaxZ = z;
				TArray<FBlockDataForGreedyMeshGeneration> BlocksOnZ = _CheckBlocksForShapeOnZ(VisitedBlocks, CurrentBlockLocation, MaxY, MaxZ);
				CurrentShape.LengthZ = (MaxZ - z) + 1;

				if (!BlocksOnZ.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnZ);

				int32 MaxX = x;
				TArray<FBlockDataForGreedyMeshGeneration> BlocksOnX = _CheckBlocksForShapeOnX(VisitedBlocks, CurrentBlockLocation, MaxY, MaxZ, MaxX);
				CurrentShape.LengthX = (MaxX - x) + 1;

				if (!BlocksOnX.IsEmpty())
					CurrentShape.Blocks.Append(BlocksOnX);

				GenerateMeshCallback(CurrentShape);
			}
		}
	}
}

void AChunk::MarchBlocks(FRealtimeMeshSimpleMeshData& MeshData)
{
	const int32 Width = m_WidthInBlocks;
	const int32 Height = m_HeightInBlocks;
	const float BlockSize = static_cast<float>(m_BlockSize);

	for (int x = -1; x < Width; x++)
	{
		for (int y = -1; y < Width; y++)
		{
			for (int z = -1; z < Height; z++)
			{
				FBlockDataForMarchingCubes Voxel;

				const FVector CurrentBlockPosition = FVector(static_cast<float>(x) * BlockSize, static_cast<float>(y) * BlockSize, static_cast<float>(z) * BlockSize) - m_OverlapCollision->GetUnscaledBoxExtent();
				_GetBlockDataForMarchingCubes(x, y, z, CurrentBlockPosition, Voxel);

				UGridUtilities::AddMeshDataFromBlock(Voxel, x, y, MeshData.Positions, MeshData.Triangles, MeshData.UV0, MeshData.Normals, MeshData.Tangents);
			}
		}
	}
}

void AChunk::_GetBlockDataForMarchingCubes(const int& x, const int& y, const int& z,
	const FVector& CurrentBlockPositionInUnits, FBlockDataForMarchingCubes& CurrentVoxel) const
{
	const float Bottom = CurrentBlockPositionInUnits.Z;
	const float Top = CurrentBlockPositionInUnits.Z + m_BlockSize;

	const float Left = CurrentBlockPositionInUnits.X;
	const float Right = CurrentBlockPositionInUnits.X + m_BlockSize;

	const float Back = CurrentBlockPositionInUnits.Y;
	const float Front = CurrentBlockPositionInUnits.Y + m_BlockSize;

	CurrentVoxel.CornerLocations[0] = FVector(Left, Back, Bottom);
	CurrentVoxel.CornerLocations[1] = FVector(Right, Back, Bottom);
	CurrentVoxel.CornerLocations[2] = FVector(Right, Front, Bottom);
	CurrentVoxel.CornerLocations[3] = FVector(Left, Front, Bottom);
	CurrentVoxel.CornerLocations[4] = FVector(Left, Back, Top);
	CurrentVoxel.CornerLocations[5] = FVector(Right, Back, Top);
	CurrentVoxel.CornerLocations[6] = FVector(Right, Front, Top);
	CurrentVoxel.CornerLocations[7] = FVector(Left, Front, Top);

	CurrentVoxel.CornersStatus[0] = UGridUtilities::IsValidBlock(_GetBlockAtLocationOptimizedForLocal(FIntVector(x, y, z)));
	CurrentVoxel.CornersStatus[1] = UGridUtilities::IsValidBlock(_GetBlockAtLocationOptimizedForLocal(FIntVector(x + 1, y, z)));
	CurrentVoxel.CornersStatus[2] = UGridUtilities::IsValidBlock(_GetBlockAtLocationOptimizedForLocal(FIntVector(x + 1, y + 1, z)));
	CurrentVoxel.CornersStatus[3] = UGridUtilities::IsValidBlock(_GetBlockAtLocationOptimizedForLocal(FIntVector(x, y + 1, z)));
	CurrentVoxel.CornersStatus[4] = UGridUtilities::IsValidBlock(_GetBlockAtLocationOptimizedForLocal(FIntVector(x, y, z + 1)));
	CurrentVoxel.CornersStatus[5] = UGridUtilities::IsValidBlock(_GetBlockAtLocationOptimizedForLocal(FIntVector(x + 1, y, z + 1)));
	CurrentVoxel.CornersStatus[6] = UGridUtilities::IsValidBlock(_GetBlockAtLocationOptimizedForLocal(FIntVector(x + 1, y + 1, z + 1)));
	CurrentVoxel.CornersStatus[7] = UGridUtilities::IsValidBlock(_GetBlockAtLocationOptimizedForLocal(FIntVector(x, y + 1, z + 1)));
}
