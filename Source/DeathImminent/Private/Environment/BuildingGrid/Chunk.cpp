// Copyright MikeSMediaStudios™ 2023

#include "Environment/BuildingGrid/Chunk.h"
#include "Environment/BuildingGrid/BuildingGrid.h"
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
}

void AChunk::UpdateMesh()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("AChunk::UpdateMesh()"))

	//std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now(), end;

	_ResetMesh();
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
	case EMeshingAlgorithm::MA_SurfaceNets:
	{
		UGridMeshUtilities::SurfaceNetsNew(MeshData);
		const UWorld* World = GetWorld();
		const FTransform& Transform = GetActorTransform();

		for(const FVector& Position : MeshData.Positions)
		{
			DrawDebugPoint(World, UGridUtilities::ConvertToWorldSpaceFVector(Position, Transform), 5.f, FColor::Red, false, 10.f);
		}
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
			if (m__Blocks[Index] == 0)
				return;

			m__Blocks[Index] = 0;
		})) return;

	UpdateMesh();
	_UpdateMeshOfSurroundingChunks(GridLocation.X, GridLocation.Y, GridLocation.Z);
}

bool AChunk::TryPlaceBlock(const FVector& Location, const FBlockID& BlockID, const bool& ShouldUpdateMeshes)
{
	FVector LocalSpaceLocation;
	FVector ArrayUsableLocation;
	FIntVector GridLocation;

	if (!__ExecuteFunctionIfIndexValid(_GetBlockIndexFromWorldLocation(Location, LocalSpaceLocation, ArrayUsableLocation, GridLocation), [this, &BlockID](const int32& Index)
		{
			if (m__Blocks[Index] != 0)
				return;

			m__Blocks[Index] = UGridUtilities::PackInt16ToInt32(BlockID.Source, BlockID.ID);
		})) return false;

	if (ShouldUpdateMeshes)
	{
		UpdateMesh();
		_UpdateMeshOfSurroundingChunks(GridLocation.X, GridLocation.Y, GridLocation.Z);
	}

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
	const int bIsXEdge = x == 0 ? -1 : x == m__WidthInBlocks - 1 ? 1 : 0;
	const int bIsYEdge = y == 0 ? -1 : y == m__WidthInBlocks - 1 ? 1 : 0;
	const int bIsZEdge = z == 0 ? -1 : z == m__WidthInBlocks - 1 ? 1 : 0;

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

FBlockID AChunk::_GetBlockAtGridLocationOptimizedForLocal(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates) const
{
	if (IsLocationInsideChunkInBlocks(x, y, z, GlobalGridCoordinates))
		return GetBlockAtGridLocation(x, y, z, GlobalGridCoordinates);

	int newX = x;
	int newY = y;
	int newZ = z;
	ConvertGridLocation(newX, newY, newZ, false);

	return m__ContainingGrid->GetBlockAtGridLocation(newX, newY, newZ);
}