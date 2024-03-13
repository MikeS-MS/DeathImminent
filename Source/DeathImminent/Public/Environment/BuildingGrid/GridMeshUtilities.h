// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
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
	static void MarchingCubes(TArray<FBlock>& Blocks, const FUpdateMeshInformation& UpdateMeshInformation, FVoxelMeshSectionData& MeshData);

private:

	static FORCEINLINE FBlockStatus __GetBlockStatus(TArray<FBlock>& Blocks,
													 const FUpdateMeshInformation& UpdateMeshInformation, 
													 const FIntVector& GridLocation, 
													 const FVector& Location)
	{
		const FBlock& Block = Blocks[UGridUtilities::Convert3DPosTo1DArrayIndex(GridLocation, UpdateMeshInformation.ChunkSize.X, UpdateMeshInformation.ChunkSize.Y)];
		return FBlockStatus(static_cast<int>(Block.BlockID != FBlock::AirID), Block.Fullness, Location, GridLocation);
	}
	static FORCEINLINE void __GetBlockStatuses(TArray<FBlock>& Blocks, 
											   const FUpdateMeshInformation& UpdateMeshInformation, 
											   const FIntVector& GridLocation,
											   const FBlockLocations& BlockLocations, 
											   TArray<FBlockStatus>& OutBlockStatuses)
	{
		const FIntVector TopFrontRight = GridLocation + FIntVector(1);

		OutBlockStatuses[0] = __GetBlockStatus(Blocks, UpdateMeshInformation, GridLocation, BlockLocations.BottomBackLeft);
		OutBlockStatuses[1] = __GetBlockStatus(Blocks, UpdateMeshInformation, FIntVector(TopFrontRight.X, GridLocation.Y, GridLocation.Z), BlockLocations.BottomBackRight());
		OutBlockStatuses[2] = __GetBlockStatus(Blocks, UpdateMeshInformation, FIntVector(TopFrontRight.X, TopFrontRight.Y, GridLocation.Z), BlockLocations.BottomFrontRight());
		OutBlockStatuses[3] = __GetBlockStatus(Blocks, UpdateMeshInformation, FIntVector(GridLocation.X, TopFrontRight.Y, GridLocation.Z), BlockLocations.BottomFrontLeft());
		OutBlockStatuses[4] = __GetBlockStatus(Blocks, UpdateMeshInformation, FIntVector(GridLocation.X, GridLocation.Y, TopFrontRight.Z), BlockLocations.TopBackLeft());
		OutBlockStatuses[5] = __GetBlockStatus(Blocks, UpdateMeshInformation, FIntVector(TopFrontRight.X, GridLocation.Y, TopFrontRight.Z), BlockLocations.TopBackRight());
		OutBlockStatuses[6] = __GetBlockStatus(Blocks, UpdateMeshInformation, TopFrontRight, BlockLocations.TopFrontRight());
		OutBlockStatuses[7] = __GetBlockStatus(Blocks, UpdateMeshInformation, FIntVector(GridLocation.X, TopFrontRight.Y, TopFrontRight.Z), BlockLocations.TopFrontLeft());
	}
	static FORCEINLINE int __GetConfigurationIndex(const TArray<FBlockStatus>& CornersStatus)
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
	static FVector __MC_InterpolatePosition(const FBlockStatus& First, const FBlockStatus& Second, const float Threshold);
	static void __AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V, TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents);

private:

	static const int sc_EdgeConfigurations[256];
	static const int sc_CornerIndexAFromEdge[12];
	static const int sc_CornerIndexBFromEdge[12];
	static const int sc_TrianglePoints[256][16];
};

