// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
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

	UFUNCTION(BlueprintCallable)
	static void SurfaceNets(FVoxelMeshSectionData& MeshData);

	UFUNCTION(BlueprintCallable)
	static void CustomMeshing(FVoxelMeshSectionData& MeshData);

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
	static FORCEINLINE void __GetBlockDataForMarchingCubes(const int& x, const int& y, const int& z, const FBlockLocations& CurrentBlockPositions, FBlockDataForMarchingCubes& CurrentVoxel);
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

	static FORCEINLINE void __AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V, TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FVector>* GenericTangents, TArray<FProcMeshTangent>* ProcMeshTangents);
#pragma endregion

	static FORCEINLINE void __GetBlockStatuses(const int& x, const int& y, const int& z, const FBlockLocations& BlockLocations, TArray<FBlockStatus>& OutBlockStatuses);

#pragma region Surface Nets Algorithm
	static FORCEINLINE void __SN_FillBlockData(const int& x, const int& y, const int& z, const int& Offset, const FBlockDataArrayForSurfaceNets& BlocksArray);
	static FORCEINLINE FBlockDataForSurfaceNets* __SN_GetBlock(const int& x, const int& y, const int& z, const FBlockDataArrayForSurfaceNets& BlocksArray)
	{
		int Index = 0;
		if (!UGridUtilities::IsValidArrayIndex(x, y, z, BlocksArray.Width, BlocksArray.Height, Index) || !BlocksArray.Blocks.IsValidIndex(Index)) return nullptr;
		return &BlocksArray.Blocks[Index];
	}
	static FORCEINLINE void __SN_CheckSurface(const int& x, const int& y, const int& z, const FBlockDataArrayForSurfaceNets& BlocksArray)
	{
		FBlockDataForSurfaceNets* BlockData = __SN_GetBlock(x, y, z, BlocksArray);
		if (!BlockData) return;
		const bool Start = BlockData->IsValid;
		for (int i = 1; i < 8; i++)
		{
			if (Start == __SN_IsValidBlock(BlockData->Corners[i])) continue;
			BlockData->IsSurface = true;
			break;
		}
	}
	static FORCEINLINE bool __SN_IsValidBlock(const FBlockDataForSurfaceNets* BlockData)
	{
		return BlockData && BlockData->IsValid;
	}
	static FORCEINLINE void __SN_FindSmoothedLocation(const int& x, const int& y, const int& z, const FBlockDataArrayForSurfaceNets& BlocksArray)
	{
		FBlockDataForSurfaceNets* BlockData = __SN_GetBlock(x, y, z, BlocksArray);
		if (!BlockData->IsSurface) return;

		int Count = 1;
		FVector Sum = BlockData->SmoothedLocation;

		for(int i = 0; i < 6; i++)
		{
			const FBlockDataForSurfaceNets* Side = BlockData->Sides[i];
			if (!Side || !Side->IsSurface) continue;
			Sum += Side->SmoothedLocation;
			Count++;
		}
		BlockData->SmoothedLocation = UGameUtilities::Clamp((Sum / static_cast<float>(Count)), BlockData->WorldLocation.BottomBackLeft, BlockData->WorldLocation.TopFrontRight());
	}
	static FORCEINLINE void __SN_SumForSmoothPosition(const FBlockDataForSurfaceNets* CurrentSide, FVector& Sum, int& Count)
	{
		if (!CurrentSide || !CurrentSide->IsSurface) return;
		Sum += CurrentSide->SmoothedLocation;
		Count++;
	}
	static FORCEINLINE void __SN_AddMeshDataFromBlock(const FBlockDataForSurfaceNets& CurrentBlockData, FVoxelMeshSectionData& MeshData);
#pragma endregion

#pragma region Custom Meshing Algorithm
	static FORCEINLINE void __CM_FillBlockData(const int& x, const int& y, const int& z, const int& Offset, const FBlockDataArrayForCustomMeshing& BlocksArray);
	static FORCEINLINE FBlockDataForCustomMeshing* __CM_GetSide(const int& x, const int& y, const int& z, const FBlockDataArrayForCustomMeshing& BlocksArray)
	{
		int Index = 0;
		if (!UGridUtilities::IsValidArrayIndex(x, y, z, BlocksArray.Width, BlocksArray.Height, Index) || !BlocksArray.Blocks.IsValidIndex(Index)) return nullptr;
		return &BlocksArray.Blocks[Index];
	}
	static FORCEINLINE void __CM_CheckSurface(const int& x, const int& y, const int& z, const FBlockDataArrayForCustomMeshing& BlocksArray)
	{
		FBlockDataForCustomMeshing& BlockData = BlocksArray.Blocks[UGridUtilities::ConvertToArrayIndex(x, y, z, BlocksArray.Width)];
		if (!BlockData.IsValid) return;

		for (int i = 0; i < 6; i++)
		{
			if (__CM_IsBlockValid(BlockData.Sides[i])) continue;

			BlockData.IsSurface = true;
			BlockData.NeedsSmoothing = true;
			break;
		}

		if (!BlockData.IsSurface) return;

		for (int i = 1; i < 8; i++)
		{
			FBlockDataForCustomMeshing* Corner = BlockData.Corners[i];
			if (!Corner) continue;
			Corner->NeedsSmoothing = true;
		}
	}
	static FORCEINLINE bool __CM_IsBlockValid(const FBlockDataForCustomMeshing* BlockData)
	{
		return BlockData && BlockData->IsValid;
	}
	static FORCEINLINE void __CM_SetNeighborsSmoothing(const int& x, const int& y, const int& z, const FBlockDataArrayForCustomMeshing& BlocksArray)
	{
		FBlockDataForCustomMeshing& BlockData = BlocksArray.Blocks[UGridUtilities::ConvertToArrayIndex(x, y, z, BlocksArray.Width)];
		if (!BlockData.IsSurface) return;

		for (int i = 0; i < 8; i++)
		{
			FBlockDataForCustomMeshing* Corner = BlockData.Corners[i];
			if (!Corner) continue;

			Corner->NeedsSmoothing = true;
		}
	}
	static FORCEINLINE void __CM_FindSmoothLocationsForBlock(const int& x, const int& y, const int& z, const FBlockDataArrayForCustomMeshing& BlocksArray)
	{
		FBlockDataForCustomMeshing& BlockData = BlocksArray.Blocks[UGridUtilities::ConvertToArrayIndex(x, y, z, BlocksArray.Width)];

		if (!BlockData.IsSurface) return;

		const FBox Constraint(BlockData.Bounds.BottomBackLeft, BlockData.Bounds.TopFrontRight());

		BlockData.IsSurface = false;
		__CM_SmoothPosition(&BlockData, Constraint);
		BlockData.IsSurface = true;

		for (int i = 1; i < 8; i++)
		{
			__CM_SmoothPosition(BlockData.Corners[i], Constraint);
		}
	}
	static FORCEINLINE void __CM_SmoothPosition(FBlockDataForCustomMeshing* BlockData, const FBox& Constraint)
	{
		if (!BlockData || BlockData->IsSurface) return;

		int Count = 1;
		FVector Sum = BlockData->SmoothedLocation;

		for (int i = 0; i < 6; i++)
		{
			const FBlockDataForCustomMeshing* Side = BlockData->Sides[i];
			if (!Side || !Side->NeedsSmoothing) continue;

			Sum += Side->SmoothedLocation;
			Count++;
		}

		BlockData->SmoothedLocation = UGameUtilities::Clamp((Sum / Count), Constraint.Min, Constraint.Max);
	}
	static FORCEINLINE bool __CM_CanSnapSmoothOnSide(const int& GridLocation, const int& Boundary, const FIntVector& Start, const FIntVector& End, const FBlockDataArrayForCustomMeshing& BlocksArray)
	{
		if (GridLocation != Boundary) return false;

		for (int x = Start.X; x <= End.X; x++)
		{
			for (int y = Start.Y; y <= End.Y; y++)
			{
				for (int z = Start.Z; z <= End.Z; z++)
				{
					const FBlockDataForCustomMeshing* BlockData = __CM_GetSide(x, y, z, BlocksArray);

					if (BlockData && BlockData->IsSurface) return true;
				}
			}
		}
		return false;
	}
	static FORCEINLINE void __CM_AddMeshDataFromBlock(const FBlockDataForCustomMeshing& BlockData, FVoxelMeshSectionData& MeshData);
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

