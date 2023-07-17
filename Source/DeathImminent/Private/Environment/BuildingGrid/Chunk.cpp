// Copyright MikeSMediaStudios™ 2023

#include "Environment/BuildingGrid/Chunk.h"
#include "Environment/BuildingGrid/BuildingGrid.h"
#include "Environment/BuildingGrid/GridMeshUtilities.h"
//#include <chrono>
#include "Net/UnrealNetwork.h"


AChunk::AChunk()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	m__ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh Component"));
	m__ProceduralMeshComponent->SetupAttachment(RootComponent);
	//m_ProceduralMeshComponent->SetCollisionProfileName("BlockAll");
	//m_ProceduralMeshComponent->EnableComplexAsSimpleCollision();
	m__OverlapCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	m__OverlapCollision->SetupAttachment(RootComponent);
	m__OverlapCollision->SetCollisionProfileName("Volume");

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> Material(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Temp/Materials/M_AlignedMaterial_Inst.M_AlignedMaterial_Inst'"));

	if (Material.Object != nullptr)
	{
		m__GridMaterial = Cast<UMaterialInstance>(Material.Object);
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
	m__ContainingGrid = ContainingGrid;
	m__ChunkPosInGrid = ChunkPosInGrid;
	m__ChunkIndexInGrid = ChunkIndexInGrid;

	m__WidthInBlocks = ContainingGrid->GetChunkWidthInBlocks();
	m__HeightInBlocks = ContainingGrid->GetChunkHeightInBlocks();
	m__BlockSize = ContainingGrid->GetBlockSize();

	m__StartGridLocation = FIntVector(StartX, StartY, StartZ);
	m__EndGridLocation = FIntVector(StartX + m__WidthInBlocks - 1, StartY + m__WidthInBlocks - 1, StartZ + m__HeightInBlocks - 1);

	const float ExtentXY = (m__WidthInBlocks * m__BlockSize) / 2;
	const float ExtentZ = (m__HeightInBlocks * m__BlockSize) / 2;

	m__OverlapCollision->SetBoxExtent(FVector(ExtentXY, ExtentXY, ExtentZ), true);
}

void AChunk::InitializeBlockArrayWithAir()
{
	m__Blocks.SetNum(UGridUtilities::GetArrayDesiredSize(m__WidthInBlocks, m__HeightInBlocks));
	__ResetPositions();
}

void AChunk::UpdateMesh()
{
	//TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("AChunk::UpdateMesh()"))

	//std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now(), end;
	_ResetMesh();
	__ResetPositions();
	UGridMeshUtilities::SurfaceNetsFindLocations(this);
	//FRealtimeMeshSimpleMeshData MeshData;
	FVoxelMeshSectionData MeshData;
	//FDynamicMesh3* MeshData = m_ProceduralMeshComponent->GetDynamicMesh()->GetMeshPtr();
	//FKAggregateGeom CollisionGeometry;
	//FKConvexElem Collision;

	//m_ProceduralMeshComponent->GetDynamicMesh()->EditMesh([this, &Collision](FDynamicMesh3& MeshData)
	//{
	UGridMeshUtilities::SetDataForMeshingAlgorithms(this);

	switch (m__ContainingGrid->MeshingAlgorithm)
	{
	case EMeshingAlgorithm::MA_CustomMeshing:
	{
		UGridMeshUtilities::CustomMeshing(MeshData);
		break;
	}
	case EMeshingAlgorithm::MA_SurfaceNets:
	{
		UGridMeshUtilities::SurfaceNets(MeshData);
		break;
	}
	case EMeshingAlgorithm::MA_MarchingCubes:
	{
		UGridMeshUtilities::MarchingCubes(MeshData);
		break;
	}
	case EMeshingAlgorithm::MA_GreedyMeshing:
	default:
	{
		UGridMeshUtilities::GreedyMeshing(MeshData);
		break;
	}
	}

	//}, EDynamicMeshChangeType::MeshChange, EDynamicMeshAttributeChangeFlags::MeshTopology);
	//CollisionGeometry.ConvexElems.Add(Collision);

	//m_ProceduralMeshComponent->GetDynamicMesh()->SetMesh(MeshData);
	//m_ProceduralMeshComponent->SetCollisionConvexMeshes(TArray<TArray<FVector>>{MeshData.Positions});
	m__ProceduralMeshComponent->CreateMeshSection_LinearColor(  0, 
																MeshData.Positions, 
																MeshData.Triangles, 
																MeshData.Normals, 
																MeshData.UVs, 
																TArray<FVector2D>(), 
																TArray<FVector2D>(), 
																TArray<FVector2D>(),
																TArray<FLinearColor>(), 
																MeshData.Tangents, 
																true );
	m__ProceduralMeshComponent->SetMaterial(0, m__GridMaterial);
	//m_ProceduralMeshComponent->SetSimpleCollisionShapes(CollisionGeometry, true);

	//m_ProceduralMeshComponent->UpdateCollision(false);
	//m_Mesh->SetupMaterialSlot(0, TEXT("Material"));
	//const FRealtimeMeshSectionGroupKey GroupKey = m_Mesh->CreateSectionGroupWithMesh(FRealtimeMeshLODKey(0), MeshData);
	//const FRealtimeMeshSectionConfig Config(ERealtimeMeshSectionDrawType::Dynamic, 0);
	//m_Mesh->CreateSectionInGroup(GroupKey, Config, FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()), true);
	//m_Mesh->UpdateCollision(true);

	//end = std::chrono::high_resolution_clock::now();
	//auto Duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//UE_LOG(LogTemp, Error, TEXT("Time taken to rebuild: %lldms Current FPS: %f"), Duration.count(), 1.0f / (UGameplayStatics::GetWorldDeltaSeconds(GetWorld()) - std::chrono::duration_cast<std::chrono::seconds>(end - start).count()));
}

void AChunk::TryHitBlock(const FVector& Location)
{
	FVector LocalSpaceLocation;
	FVector ArrayUsableLocation;
	FIntVector GridLocation;

	if (!__ExecuteFunctionIfIndexValid(_GetBlockIndexFromWorldLocation(Location, LocalSpaceLocation, ArrayUsableLocation, GridLocation), [this](const int32& Index)
		{
			Block& Block = m__Blocks[Index];
			if (!Block.IsSolid)
				return;

			Block.Source = FBlockID::Air.Source;
			Block.ID = FBlockID::Air.ID;
			Block.IsSolid = false;
		})) return;
	__SetBlockStatus(GridLocation.X, GridLocation.Y, GridLocation.Z, true);
}

bool AChunk::TryPlaceBlock(const FVector& Location, const FBlockID& BlockID, const bool& ShouldUpdateMeshes)
{
	FVector LocalSpaceLocation;
	FVector ArrayUsableLocation;
	FIntVector GridLocation;

	if (!__ExecuteFunctionIfIndexValid(_GetBlockIndexFromWorldLocation(Location, LocalSpaceLocation, ArrayUsableLocation, GridLocation), [this, &BlockID](const int32& Index)
		{
			Block& Block = m__Blocks[Index];
			if (!Block.IsSolid)
				return;

			Block.Source = BlockID.Source;
			Block.ID = BlockID.ID;
			Block.IsSolid = true;
		})) return false;

	__SetBlockStatus(GridLocation.X, GridLocation.Y, GridLocation.Z, ShouldUpdateMeshes);
	return true;
}

bool AChunk::SetBlock(const FBlockID& BlockID, const int& x, const int& y, const int& z,
	const bool& GlobalGridCoordinates, const bool ShouldUpdateMesh)
{
	int newX = x, newY = y, newZ = z;

	if (GlobalGridCoordinates)
		ConvertGridLocation(newX, newY, newZ, GlobalGridCoordinates);

	const int32 BlockIndex = UGridUtilities::ConvertToArrayIndex(newX, newY, newZ, m__WidthInBlocks);
	if (!__ExecuteFunctionIfIndexValid(BlockIndex, [this, &BlockID](const int32& Index)
	{
		Block& Block = m__Blocks[Index];
		Block.Source = BlockID.Source;
		Block.ID = BlockID.ID;
		Block.IsSolid = true;
	})) return false;

	__SetBlockStatus(newX, newY, newZ, ShouldUpdateMesh);

	return true;
}

void AChunk::_ResetMesh()
{
	//m_Mesh = m_ProceduralMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();
	//m_ProceduralMeshComponent->GetDynamicMesh()->Reset();
	m__ProceduralMeshComponent->ClearAllMeshSections();
}

void AChunk::_UpdateMeshOfSurroundingChunks(const int& x, const int& y, const int& z)
{
	const auto& IsInsideBounds = [](const int& Position, const int& Min, const int& Max)
	{
		return Position >= Min && Position <= Max;
	};
	const int& BlocksAhead = m__ContainingGrid->LookAheadBlocks;
	const int LastIndexXY = m__WidthInBlocks - 1;
	const int LastIndexZ = m__HeightInBlocks - 1;

	const int bIsXEdge = IsInsideBounds(x, 0, BlocksAhead - 1) ? -1 : IsInsideBounds(x, (LastIndexXY - BlocksAhead), LastIndexXY) ? 1 : 0;
	const int bIsYEdge = IsInsideBounds(y, 0, BlocksAhead - 1) ? -1 : IsInsideBounds(y, (LastIndexXY - BlocksAhead), LastIndexXY) ? 1 : 0;
	const int bIsZEdge = IsInsideBounds(z, 0, BlocksAhead - 1) ? -1 : IsInsideBounds(z, (LastIndexZ - BlocksAhead), LastIndexZ) ? 1 : 0;

	TArray<FIntVector> ChunkUpdateOrder;
	ChunkUpdateOrder.Reserve(6);

	if (bIsXEdge != 0)
	{
		AChunk* LeftRightChunk = m__ContainingGrid->GetChunkAtArrayPosition(FIntVector(m__ChunkPosInGrid.X + bIsXEdge, m__ChunkPosInGrid.Y, m__ChunkPosInGrid.Z));
		if (IsValid(LeftRightChunk))
			LeftRightChunk->UpdateMesh();
	}

	if (bIsYEdge != 0)
	{
		AChunk* FrontBackChunk = m__ContainingGrid->GetChunkAtArrayPosition(FIntVector(m__ChunkPosInGrid.X, m__ChunkPosInGrid.Y + bIsYEdge, m__ChunkPosInGrid.Z));
		if (IsValid(FrontBackChunk))
			FrontBackChunk->UpdateMesh();
	}

	if (bIsZEdge != 0)
	{
		AChunk* BottomTopChunk = m__ContainingGrid->GetChunkAtArrayPosition(FIntVector(m__ChunkPosInGrid.X, m__ChunkPosInGrid.Y, m__ChunkPosInGrid.Z + bIsZEdge));
		if (IsValid(BottomTopChunk))
			BottomTopChunk->UpdateMesh();
	}

	if ((bIsXEdge != 0 && bIsYEdge != 0) && bIsZEdge != 0)
	{
		AChunk* CornerChunk = m__ContainingGrid->GetChunkAtArrayPosition(FIntVector(m__ChunkPosInGrid.X + bIsXEdge, m__ChunkPosInGrid.Y + bIsYEdge, m__ChunkPosInGrid.Z + bIsZEdge));
		if (IsValid(CornerChunk))
			CornerChunk->UpdateMesh();
	}

	if (bIsXEdge != 0 && bIsYEdge != 0)
	{
		AChunk* EdgeChunk = m__ContainingGrid->GetChunkAtArrayPosition(FIntVector(m__ChunkPosInGrid.X + bIsXEdge, m__ChunkPosInGrid.Y + bIsYEdge, m__ChunkPosInGrid.Z));
		if (IsValid(EdgeChunk))
			EdgeChunk->UpdateMesh();
	}

	if (bIsXEdge != 0 && bIsZEdge != 0)
	{
		AChunk* EdgeChunk = m__ContainingGrid->GetChunkAtArrayPosition(FIntVector(m__ChunkPosInGrid.X + bIsXEdge, m__ChunkPosInGrid.Y, m__ChunkPosInGrid.Z + bIsZEdge));
		if (IsValid(EdgeChunk))
			EdgeChunk->UpdateMesh();
	}

	if (bIsYEdge != 0 && bIsZEdge != 0)
	{
		AChunk* EdgeChunk = m__ContainingGrid->GetChunkAtArrayPosition(FIntVector(m__ChunkPosInGrid.X, m__ChunkPosInGrid.Y + bIsYEdge, m__ChunkPosInGrid.Z + bIsZEdge));
		if (IsValid(EdgeChunk))
			EdgeChunk->UpdateMesh();
	}
}

Block* AChunk::_GetBlockAtGridLocationOptimizedForLocal(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates)
{
	if (IsLocationInsideChunkInBlocks(x, y, z, GlobalGridCoordinates))
		return GetBlockAtGridLocation(x, y, z, GlobalGridCoordinates);

	int newX = x;
	int newY = y;
	int newZ = z;
	ConvertGridLocation(newX, newY, newZ, false);

	return m__ContainingGrid->GetBlockAtGridLocation(newX, newY, newZ);
}

void AChunk::__SetBlockStatus(const int& x, const int& y, const int& z, const bool& ShouldUpdate)
{
	const auto& CheckSurface = [this](const int& x, const int& y, const int& z, const bool& StartingPoint)
	{
		for (int XStart = x; XStart <= x + 1; XStart++)
		{
			for (int YStart = y; YStart <= y + 1; YStart++)
			{
				for (int ZStart = z; ZStart <= z + 1; ZStart++)
				{
					Block* Block = _GetBlockAtGridLocationOptimizedForLocal(XStart, YStart, ZStart);
					if ((Block && Block->IsSolid) != StartingPoint) return true;
				}
			}
		}

		return false;
	};

	for(int XStart = x - 1;  XStart <= x; XStart++)
	{
		for (int YStart = y - 1; YStart <= y; YStart++)
		{
			for (int ZStart = z - 1; ZStart <= z; ZStart++)
			{
				Block* Block = _GetBlockAtGridLocationOptimizedForLocal(XStart, YStart, ZStart);
				if (!Block) continue;
				Block->IsSurface = CheckSurface(XStart, YStart, ZStart, Block && Block->IsSolid);
			}
		}
	}

	if (!ShouldUpdate) return;

	_UpdateMeshOfSurroundingChunks(x, y, z);
	UpdateMesh();
}
