// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GridUtilities.h"
#include "Utilities/GameUtilities.h"
#include "GridMeshUtilities.generated.h"

class AChunk;

UCLASS()
class DEATHIMMINENT_API UGridMeshUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static void SetDataForMeshingAlgorithms(AChunk* Chunk);

	UFUNCTION(BlueprintCallable)
	static void GreedyMeshing(FVoxelMeshSectionData& MeshData);

	UFUNCTION(BlueprintCallable)
	static void MarchingCubes(FVoxelMeshSectionData& MeshData);

	//UFUNCTION(BlueprintCallable)
	//static void SurfaceNets(FVoxelMeshSectionData& MeshData);

	static void SurfaceNetsRework(FVoxelMeshSectionData& MeshData);

	UFUNCTION(BlueprintCallable)
	static void SurfaceNetsNew(FVoxelMeshSectionData& MeshData);

private:

#pragma region Greedy Meshing Algorithm
	static FORCEINLINE bool __IsBlockValidForShape(TArray<bool>& VisitedBlocks, const FIntVector& Location, FBlockDataForGreedyMeshGeneration& OutBlockData);
	static FORCEINLINE TArray<FBlockDataForGreedyMeshGeneration> __CheckBlocksForShapeOnY(TArray<bool>& VisitedBlocks, const FIntVector& Location, int32& OutMaxY);
	static FORCEINLINE TArray<FBlockDataForGreedyMeshGeneration> __CheckBlocksForShapeOnZ(TArray<bool>& VisitedBlocks, const FIntVector& Location, const int32& MaxY, int32& OutMaxZ);
	static FORCEINLINE TArray<FBlockDataForGreedyMeshGeneration> __CheckBlocksForShapeOnX(TArray<bool>& VisitedBlocks, const FIntVector& Location, const int32& MaxY, const int32& MaxZ, int32& OutMaxX);
	static FORCEINLINE void __GenerateBox(const FGreedyMeshShape& Shape, FVoxelMeshSectionData& MeshData);
	static FORCEINLINE FBlockDataForGreedyMeshGeneration __GetBlockDataForGreedyMeshing(const FIntVector& Location);

	//static void AppendBoxMesh(const FVector& BoxRadius, const FTransform& BoxTransform, UE::Geometry::FDynamicMesh3& MeshData);
	static void AppendBoxMesh(const FVector& BoxRadius, const FTransform& BoxTransform, FVoxelMeshSectionData& MeshData);
#pragma endregion

#pragma region Marching Cubes Algorithm
	static void __GetBlockDataForMarchingCubes(const int& x, const int& y, const int& z, const FBlockLocations& CurrentBlockPositions, FBlockDataForMarchingCubes& CurrentVoxel);
	template<typename T>
	static FORCEINLINE int __GetConfigurationIndex(const TArray<T>& CornersStatus)
	{
		if (CornersStatus.Num() != 8)
			return 0;

		int ConfigIndex = 0;
		int CurrentConfigNumber = 1;

		for (int i = 0; i < 8; i++)
		{
			const int Status = CornersStatus[i].Status;
			if (Status != 0)
			{
				ConfigIndex |= CurrentConfigNumber;
			}
			CurrentConfigNumber *= 2;
		}

		return ConfigIndex;
	}

	static FORCEINLINE void __AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V, TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FVector>& Tangents);
	static FORCEINLINE void __AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V, TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents);
	//static void AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V, UE::Geometry::FDynamicMesh3& MeshData, FKConvexElem& Collision);

	static void __AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V, TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FVector>* GenericTangents, TArray<FProcMeshTangent>* ProcMeshTangents);
#pragma endregion

	static void __GetBlockStatuses(const int& x, const int& y, const int& z, const FBlockLocations& BlockLocations, TArray<FBlockStatus>& OutBlockStatuses);

#pragma region Surface Nets Algorithm
	static FORCEINLINE void __GetBlockDataForSurfaceNets(const int& x, const int& y, const int& z, const FBlockLocations& BlockLocations, FBlockDataForSurfaceNets& BlockData)
	{
		__GetBlockStatuses(x, y, z, BlockLocations, BlockData.Corners);
		BlockData.Configuration = __GetConfigurationIndex(BlockData.Corners);
		BlockData.IsSurface = sc_EdgeConfigurations[BlockData.Configuration] != 0;
		BlockData.IsAir = BlockData.Configuration == 0 ? true : false;
		BlockData.WorldLocation = BlockLocations;
		BlockData.GridLocation = FIntVector(x, y, z);
		__FindLocationForCurrentBlock(BlockData);
	}
	static FORCEINLINE void __FindLocationForCurrentBlock(FBlockDataForSurfaceNets& BlockData)
	{
		BlockData.SmoothedLocation = BlockData.WorldLocation.BottomBackLeft;
		return;
		if (!BlockData.IsSurface)
			return;

		//FVector Sum = BlockData.WorldLocation.BottomBackLeft;
		FVector Sum = FVector::Zero();
		int Count = 0;

		for (int i = 0; i < 8; i++)
		{
			const FBlockStatus& CurrentBlockStatus = BlockData.Corners[i];

			if (!CurrentBlockStatus.Status)
				continue;


			Sum += CurrentBlockStatus.Location;
			Count++;
		}

		if (Count < 1)
			return;

		BlockData.SmoothedLocation = UGameUtilities::Clamp((Sum / static_cast<float>(Count)), BlockData.WorldLocation.BottomBackLeft, BlockData.WorldLocation.TopFrontRight());
	}
	static FORCEINLINE void __AddMeshDataFromBlock(const FBlockDataForSurfaceNets& CurrentBlockData, const TArray<FBlockDataForSurfaceNets>& Blocks, const int& BlockArrayWidth, FVoxelMeshSectionData& MeshData);
#pragma endregion

private:

	static int32& sm_Width;
	static int32& sm_Height;
	static int32& sm_BlockSize;
	static FTransform& sm_OwnerTransform;
	static FVector& sm_OwnerLocation;
	static FVector& sm_OwnerExtent;
	static TArray<int32>& sm_BlocksArray;
	static AChunk* sm_Chunk;

	static const int sc_EdgeConfigurations[256];
	static const int sc_CornerIndexAFromEdge[12];
	static const int sc_CornerIndexBFromEdge[12];
	static const int sc_TrianglePoints[256][16];
};

