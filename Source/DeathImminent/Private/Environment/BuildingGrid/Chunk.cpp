// Copyright MikeSMediaStudios™ 2023

#include "Environment/BuildingGrid/Chunk.h"
#include "Environment/BuildingGrid/BuildingGrid.h"
#include "Environment/BuildingGrid/GridMeshUtilities.h"
#include "Environment/BuildingGrid/VoxelDataMeshStructs.h"
//#include <chrono>
#include "Environment/BuildingGrid/ChunkUpdateQueueComponent.h"
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

void AChunk::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AChunk::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Conditions;
	Conditions.Condition = COND_InitialOnly;
	DOREPLIFETIME(AChunk, m__ChunkLocationData)
	DOREPLIFETIME(AChunk, m__ChunkData)
}

void AChunk::InitializeBlockArrayWithAir()
{
	const FIntVector SizeInBlocks = m__ContainingGrid->GetGridData().ChunkSizeInBlocks;
	m__ChunkData.Blocks.SetNum(UGridUtilities::GetArrayDesiredSize3D(SizeInBlocks.X, SizeInBlocks.Y, SizeInBlocks.Z));
}

FBlockLocations AChunk::ConvertBlockGridPosToWorldPos(const FIntVector& GridLocation)
{
	return UGridUtilities::ConvertBlockGridPosToWorldPos(GridLocation, m__ContainingGrid->GetGridData().BlockSize, m__OverlapCollision->GetUnscaledBoxExtent());
}

bool AChunk::IsLocationInsideChunkGridLocation(const FIntVector& GridLocation, const bool GlobalGridCoordinates) const
{
	if (GlobalGridCoordinates)
		return IsLocationInsideUsingGridLocation(GridLocation, m__ChunkLocationData.StartGridLocation, m__ChunkLocationData.EndGridLocation);

	return IsLocationInsideUsingGridLocation(GridLocation, FIntVector(0, 0, 0), m__ContainingGrid->GetGridData().ChunkSizeInBlocks);
}

void AChunk::_SetInformation(ABuildingGrid* ContainingGrid, const FChunkLocationData& ChunkLocationData)
{
	m__ChunkLocationData = ChunkLocationData;
	m__ContainingGrid = ContainingGrid;
	m__OverlapCollision->SetBoxExtent(FVector(ContainingGrid->GetGridData().ChunkSizeInUnits / 2), true);
}

void AChunk::_SetShouldUpdate()
{
	if (!IsValid(m__ContainingGrid))
		return; // TODO: Ideally we want to close the game as something super wrong is going on if the grid doesn't exist

	FUpdateMeshInformation UpdateMeshInformation;
	UpdateMeshInformation.BlockSize = m__ContainingGrid->GetGridData().BlockSize;
	UpdateMeshInformation.ChunkSize = m__ContainingGrid->GetGridData().ChunkSizeInBlocks + FIntVector(2);
	UpdateMeshInformation.UnscaledBoxExtent = m__OverlapCollision->GetUnscaledBoxExtent();

	FVoxelMeshSectionData MeshData;
	TArray<FBlock> Blocks = __CopyRelevantBlocksForUpdate();
	UGridMeshUtilities::MarchingCubes(Blocks, UpdateMeshInformation, MeshData);
	_UpdateMesh(MeshData);
	//m__ContainingGrid->m_ChunkUpdateQueueComponent->__EnqueueChunk(this);
}

void AChunk::_UpdateMesh(const FVoxelMeshSectionData& MeshData)
{
	_ResetMesh();

	m__ProceduralMeshComponent->CreateMeshSection_LinearColor(0,
															  MeshData.Positions,
															  MeshData.Triangles,
															  MeshData.Normals,
															  MeshData.UVs,
															  TArray<FVector2D>(),
															  TArray<FVector2D>(),
															  TArray<FVector2D>(),
															  TArray<FLinearColor>(),
															  MeshData.Tangents,
															  true);
	m__ProceduralMeshComponent->SetMaterial(0, m__GridMaterial);

	//TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("AChunk::_UpdateMesh()"))

	//std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now(), end;
	
	//__RecalculateValues();
	//UGridMeshUtilities::SurfaceNetsFindLocations(this);
	//FRealtimeMeshSimpleMeshData MeshData;
	//FDynamicMesh3* MeshData = m_ProceduralMeshComponent->GetDynamicMesh()->GetMeshPtr();
	//FKAggregateGeom CollisionGeometry;
	//FKConvexElem Collision;

	//m_ProceduralMeshComponent->GetDynamicMesh()->EditMesh([this, &Collision](FDynamicMesh3& MeshData)
	//{

	//}, EDynamicMeshChangeType::MeshChange, EDynamicMeshAttributeChangeFlags::MeshTopology);
	//CollisionGeometry.ConvexElems.Add(Collision);

	//m_ProceduralMeshComponent->GetDynamicMesh()->SetMesh(MeshData);
	//m_ProceduralMeshComponent->SetCollisionConvexMeshes(TArray<TArray<FVector>>{MeshData.Positions});

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

void AChunk::_ResetMesh()
{
	//m_Mesh = m_ProceduralMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();
	//m_ProceduralMeshComponent->GetDynamicMesh()->Reset();
	m__ProceduralMeshComponent->ClearAllMeshSections();
}

void AChunk::_UpdateMeshOfSurroundingChunks(const FIntVector& GridLocation, TArray<AChunk*>& ChunksToBeUpdated)
{
	const FIntVector LastIndex = m__ContainingGrid->GetGridData().ChunkSizeInBlocks - FIntVector(1);

	const int bIsXEdge = GridLocation.X == 0 ? -1 : GridLocation.X == LastIndex.X ? 1 : 0;
	const int bIsYEdge = GridLocation.Y == 0 ? -1 : GridLocation.Y == LastIndex.Y ? 1 : 0;
	const int bIsZEdge = GridLocation.Z == 0 ? -1 : GridLocation.Z == LastIndex.Z ? 1 : 0;

	if (bIsXEdge != 0)
	{
		AChunk* LeftRightChunk = m__ContainingGrid->_GetChunkChunkArrayLocation(FIntVector(m__ChunkLocationData.PositionInGrid.X + bIsXEdge, m__ChunkLocationData.PositionInGrid.Y, m__ChunkLocationData.PositionInGrid.Z));
		if (IsValid(LeftRightChunk))
		{
			ChunksToBeUpdated.AddUnique(LeftRightChunk);
			LeftRightChunk->_SetShouldUpdate();
		}
	}

	if (bIsYEdge != 0)
	{
		AChunk* FrontBackChunk = m__ContainingGrid->_GetChunkChunkArrayLocation(FIntVector(m__ChunkLocationData.PositionInGrid.X, m__ChunkLocationData.PositionInGrid.Y + bIsYEdge, m__ChunkLocationData.PositionInGrid.Z));
		if (IsValid(FrontBackChunk))
		{
			ChunksToBeUpdated.AddUnique(FrontBackChunk);
			FrontBackChunk->_SetShouldUpdate();
		}
	}

	if (bIsZEdge != 0)
	{
		AChunk* BottomTopChunk = m__ContainingGrid->_GetChunkChunkArrayLocation(FIntVector(m__ChunkLocationData.PositionInGrid.X, m__ChunkLocationData.PositionInGrid.Y, m__ChunkLocationData.PositionInGrid.Z + bIsZEdge));
		if (IsValid(BottomTopChunk))
		{
			ChunksToBeUpdated.AddUnique(BottomTopChunk);
			BottomTopChunk->_SetShouldUpdate();
		}
	}

	if ((bIsXEdge != 0 && bIsYEdge != 0) && bIsZEdge != 0)
	{
		AChunk* CornerChunk = m__ContainingGrid->_GetChunkChunkArrayLocation(FIntVector(m__ChunkLocationData.PositionInGrid.X + bIsXEdge, m__ChunkLocationData.PositionInGrid.Y + bIsYEdge, m__ChunkLocationData.PositionInGrid.Z + bIsZEdge));
		if (IsValid(CornerChunk))
		{
			ChunksToBeUpdated.AddUnique(CornerChunk);
			CornerChunk->_SetShouldUpdate();
		}
	}

	if (bIsXEdge != 0 && bIsYEdge != 0)
	{
		AChunk* EdgeChunk = m__ContainingGrid->_GetChunkChunkArrayLocation(FIntVector(m__ChunkLocationData.PositionInGrid.X + bIsXEdge, m__ChunkLocationData.PositionInGrid.Y + bIsYEdge, m__ChunkLocationData.PositionInGrid.Z));
		if (IsValid(EdgeChunk))
		{
			ChunksToBeUpdated.AddUnique(EdgeChunk);
			EdgeChunk->_SetShouldUpdate();
		}
	}

	if (bIsXEdge != 0 && bIsZEdge != 0)
	{
		AChunk* EdgeChunk = m__ContainingGrid->_GetChunkChunkArrayLocation(FIntVector(m__ChunkLocationData.PositionInGrid.X + bIsXEdge, m__ChunkLocationData.PositionInGrid.Y, m__ChunkLocationData.PositionInGrid.Z + bIsZEdge));
		if (IsValid(EdgeChunk))
		{
			ChunksToBeUpdated.AddUnique(EdgeChunk);
			EdgeChunk->_SetShouldUpdate();
		}
	}

	if (bIsYEdge != 0 && bIsZEdge != 0)
	{
		AChunk* EdgeChunk = m__ContainingGrid->_GetChunkChunkArrayLocation(FIntVector(m__ChunkLocationData.PositionInGrid.X, m__ChunkLocationData.PositionInGrid.Y + bIsYEdge, m__ChunkLocationData.PositionInGrid.Z + bIsZEdge));
		if (IsValid(EdgeChunk))
		{
			ChunksToBeUpdated.AddUnique(EdgeChunk);
			EdgeChunk->_SetShouldUpdate();
		}
	}
}

int32 AChunk::_GetBlockIndexFromGridLocation(const FIntVector& GridLocation) const
{
	const FIntVector BlockSize = m__ContainingGrid->GetGridData().ChunkSizeInBlocks;
	return UGridUtilities::Convert3DPosTo1DArrayIndex(GridLocation, BlockSize.X, BlockSize.Y);
}

FBlock AChunk::_GetBlockAtGridLocation(const FIntVector& GridLocation, bool& Result, const bool GlobalCoordinates) const
{
	FIntVector NewLocation = GridLocation;

	if (GlobalCoordinates)
		ConvertGridLocation(NewLocation, GlobalCoordinates);

	const int32 Index = _GetBlockIndexFromGridLocation(NewLocation);
	if (!m__ChunkData.Blocks.IsValidIndex(Index))
	{
		Result = false;
		return FBlock::Air;
	}

	Result = true;
	return m__ChunkData.Blocks[Index];
}

void AChunk::OnRep_ChunkData()
{
	_SetShouldUpdate();

	for (AChunk* Chunk : m__ChunkData.ChunksNeededToBeUpdated)
	{
		if (!IsValid(Chunk))
			continue;

		Chunk->_SetShouldUpdate();
	}
}

TArray<FBlock> AChunk::__CopyRelevantBlocksForUpdate() const
{
	bool Result = false;
	// We are using ChunkSizeInBlocks + 2, so we can grab the last blocks from the previous chunks and the first blocks from the next chunks
	const FIntVector ChunkSizeInBlocks = m__ContainingGrid->GetGridData().ChunkSizeInBlocks + FIntVector(2);
	TArray<FBlock> Blocks;
	Blocks.SetNum(UGridUtilities::GetArrayDesiredSize3D(ChunkSizeInBlocks.X, ChunkSizeInBlocks.Y, ChunkSizeInBlocks.Z));

	for(int x = m__ChunkLocationData.StartGridLocation.X - 1, StartX = 0; x <= m__ChunkLocationData.EndGridLocation.X + 1; x++, StartX++)
	{
		for (int y = m__ChunkLocationData.StartGridLocation.Y - 1, StartY = 0; y <= m__ChunkLocationData.EndGridLocation.Y + 1; y++, StartY++)
		{
			for (int z = m__ChunkLocationData.StartGridLocation.Z - 1, StartZ = 0; z <= m__ChunkLocationData.EndGridLocation.Z + 1; z++, StartZ++)
			{
				FIntVector GridLocation(x, y, z);
				//ConvertGridLocation(GridLocation, false);
				Blocks[UGridUtilities::Convert3DPosTo1DArrayIndex(FIntVector(StartX, StartY, StartZ), ChunkSizeInBlocks.X, ChunkSizeInBlocks.Y)] = m__ContainingGrid->_GetBlockAtGridLocation(GridLocation, Result);
			}
		}
	}

	return Blocks;
}

FSetBlockOperationResult AChunk::__SetBlockAtGridLocation(const FIntVector& GridLocation,
                                                          const FBaseID& BlockID, 
														  const double Fullness, 
														  const bool GlobalCoordinates, 
														  const bool Increments, 
														  const bool Force, 
														  const bool ShouldUpdate)
{
	FSetBlockOperationResult Result;

	if (!HasAuthority())
		return Result;

	if (!IsValid(BlockID))
		return Result;

	int32 Index = 0;
	FIntVector CorrectedPos;
	if (!IsLocationInsideChunkGridLocationAndBlockExists(GridLocation, Index, CorrectedPos, GlobalCoordinates))
		return Result;

	FBlock& Block = m__ChunkData.Blocks[Index];
	Result.BlockIDPreChange = Block.BlockID;
	Result.BlockIDPostChange = Block.BlockID;
	Result.BlockFullnessPreChange = Block.Fullness;
	Result.BlockFullnessPostChange = Block.Fullness;

	const bool IsBlockAir = Block.BlockID == FBlock::AirID;
	if (((Block.BlockID == BlockID || IsBlockAir) && (!Block.IsFull() && Fullness > 0.0)) || Force)
	{
		Result.Completed = true;

		if (IsBlockAir || Force)
			Block.BlockID = BlockID;

		Result.BlockIDPostChange = Block.BlockID;

		if (Increments)
			Result.BlockFullnessPostChange = Block.SetFullness(Block.Fullness + Fullness, Result.LeftOverFullness);
		else
			Result.BlockFullnessPostChange = Block.SetFullness(Fullness, Result.LeftOverFullness);

		if (ShouldUpdate)
		{
			_SetShouldUpdate();
			_UpdateMeshOfSurroundingChunks(CorrectedPos, m__ChunkData.ChunksNeededToBeUpdated);
		}
	}

	return Result;
}

//FBlock& AChunk::_GetBlockAtGridLocationOptimizedForLocal(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates)
//{
//	if (IsLocationInsideChunkGridLocation(FIntVector(x, y, z), GlobalGridCoordinates))
//		return GetBlockAtGridLocation(x, y, z, GlobalGridCoordinates);
//
//	int newX = x;
//	int newY = y;
//	int newZ = z;
//	ConvertGridLocation(newX, newY, newZ, false);
//
//	return m__ContainingGrid->GetBlockAtGridLocation(newX, newY, newZ);
//}