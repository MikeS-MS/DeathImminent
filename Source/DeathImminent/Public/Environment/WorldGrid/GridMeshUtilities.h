// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utilities/GameUtilities.h"
#include "GridMeshUtilities.generated.h"

class AChunk;

UCLASS()
class DEATHIMMINENT_API UGridMeshUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static void MarchingCubes(AChunk* Chunk, FVoxelMeshSectionData& MeshData, bool& StopEarly);
	
	UFUNCTION(BlueprintCallable)
	static void SurfaceNets(AChunk* Chunk, FVoxelMeshSectionData& MeshData, bool& StopEarly);

private:

	static FORCEINLINE int __GetConfigurationIndex(AChunk* Chunk, 
												   TArray<const FBlock*>& CachedBlocks, 
												   const int32 X, 
												   const int32 Y, 
												   const int32 Z, 
												   const FIntVector& ChunkSize, 
												   bool& StopEarly)
	{
		//TRACE_CPUPROFILER_EVENT_SCOPE_STR(TEXT("UGridMeshUtilities::__GetConfigurationIndex()"))

		int ConfigIndex = 0;
		int CurrentConfigNumber = 1;

		for (int i = 0; i < 8; i++)
		{
			if (StopEarly)
				return 0;

			const FIntVector& CurrentOffset = sc_CornerOffsets[i];
			CachedBlocks[i] = &Chunk->_GetBlockAtGridLocation_OptimizedForLocal(X + CurrentOffset.X, Y + CurrentOffset.Y, Z + CurrentOffset.Z, ChunkSize);
			if (!CachedBlocks[i]->IsAir())
			{
				ConfigIndex |= CurrentConfigNumber;
			}
			CurrentConfigNumber *= 2;
		}

		return ConfigIndex;
	}

	static FORCEINLINE FVector __MC_InterpolatePosition(const bool FirstValid, 
														const double FirstValue, 
														const double SecondValue, 
														const FVector& FirstLocation, 
														const FVector& SecondLocation)
	{
		const bool bIsFirstValid = FirstValid;
		const FVector FirstLocationChecked = bIsFirstValid ? FirstLocation : SecondLocation;
		const FVector SecondLocationChecked = bIsFirstValid ? SecondLocation : FirstLocation;
		const double InterpolationValue = bIsFirstValid ? FirstValue : SecondValue;
		return (FMath::VInterpTo(FirstLocationChecked, SecondLocationChecked, 1.f, InterpolationValue));
	}	
	
	static FORCEINLINE FVector __MC_InterpolatePositionExperimental(const float ThreshHold,	
																	const float FirstValue, 
																	const float SecondValue, 
																	const FVector& FirstLocation, 
																	const FVector& SecondLocation)
	{
		float t = 0.0f;
		if (FirstValue > 0.0f && SecondValue > 0.0f)
			t = (ThreshHold - FirstValue) / (SecondValue - FirstValue);
		else if (FirstValue > 0.0f)
		{
			float FirstLeftOver = 1.0f - FirstValue;
			t = (ThreshHold - FirstValue) / (FirstLeftOver - FirstValue);
		}
		else if (SecondValue > 0.0f)
		{
			float SecondLeftOver = 1.0f - SecondValue;
			t = (ThreshHold - SecondValue) / (SecondLeftOver - SecondValue);
		}
		
		return (FMath::VInterpTo(FirstLocation, SecondLocation, 1.f, t));
	}		
	
	static FORCEINLINE FVector __MC_InterpolatePositionExperimental2(const float FirstValue, 
																	 const float SecondValue, 
																	 const FVector& FirstLocation, 
																	 const FVector& SecondLocation)
	{
		if (FirstValue < 0.00001f && SecondValue < 0.00001f)
			return FirstLocation;
		else if (FirstValue < 0.00001f)
			return (FMath::VInterpTo(FirstLocation, SecondLocation, 1.f, SecondValue));
		else if (SecondValue < 0.00001f)
			return (FMath::VInterpTo(FirstLocation, SecondLocation, 1.f, FirstValue));  
		
		return (FMath::VInterpTo(FirstLocation, SecondLocation, 1.f, FirstValue));
	}	
	
	static FORCEINLINE FVector __MC_InterpolatePositionThreshold(const float ThreshHold, 
																 const float FirstValue, 
																 const float SecondValue, 
																 const FVector& FirstLocation, 
																 const FVector& SecondLocation)
	{
		// const bool bIsFirstValid = FirstValid;
		float t = 0.0f;
		float firstDifference = FMath::Abs(ThreshHold - FirstValue);
		float secondDifference = FMath::Abs(ThreshHold - SecondValue);
		if (firstDifference < 0.00001f && secondDifference < 0.00001f)
			return FirstLocation;
		else if (firstDifference < 0.00001f)
			t = SecondValue;		
		else if (secondDifference < 0.00001f)
			t = FirstValue;
		else if (FMath::Abs(FirstValue - SecondValue) < 0.00001f)
			t = FirstValue;
		else
			t = (ThreshHold - FirstValue) / (SecondValue - FirstValue);
		// const FVector FirstLocationChecked = bIsFirstValid ? FirstLocation : SecondLocation;
		// const FVector SecondLocationChecked = bIsFirstValid ? SecondLocation : FirstLocation;
		// const double InterpolationValue = bIsFirstValid ? FirstValue : SecondValue;
		return (FMath::VInterpTo(FirstLocation, SecondLocation, 1.f, t));
	}
	
	static FORCEINLINE FSurfaceNetsVoxelData __SN_CreateBlockData(const AChunk* Chunk, const FIntVector& CurrentBlockLocation, const FIntVector& ChunkSize)
	{
		const FBlock& Block = Chunk->_GetBlockAtGridLocation_OptimizedForLocal(CurrentBlockLocation.X, CurrentBlockLocation.Y, CurrentBlockLocation.Z, ChunkSize);
		FSurfaceNetsVoxelData BlockData(Chunk->ConvertBlockGridPosToWorldPos(CurrentBlockLocation), Block, CurrentBlockLocation);
		return BlockData;
	}
	
	static FORCEINLINE const FSurfaceNetsVoxelData* __SN_GetBlock(const AChunk* Chunk, const FIntVector& CurrentBlockLocation, const FIntVector& ChunkSize, TMap<FIntVector, FSurfaceNetsVoxelData>& BlocksMap)
	{
		if (BlocksMap.Contains(CurrentBlockLocation))
			return &BlocksMap[CurrentBlockLocation];
		
		BlocksMap.Add(CurrentBlockLocation, __SN_CreateBlockData(Chunk, CurrentBlockLocation, ChunkSize));
		return &BlocksMap[CurrentBlockLocation];
	}
	
	static FORCEINLINE bool __SN_DetermineIsSurface(const FSurfaceNetsVoxelData& BlockData)
	{
		if (!BlockData.IsValid()) return false;
		if (!BlockData.Left   || !BlockData.Left->IsValid()) return true;
		if (!BlockData.Right  || !BlockData.Right->IsValid()) return true;
		if (!BlockData.Back   || !BlockData.Back->IsValid()) return true;
		if (!BlockData.Front  || !BlockData.Front->IsValid()) return true;
		if (!BlockData.Bottom || !BlockData.Bottom->IsValid()) return true;
		if (!BlockData.Top    || !BlockData.Top->IsValid()) return true;
		return false;
	}
	
	static FORCEINLINE void __SN_FillBlockData(const AChunk* Chunk, const FIntVector& CurrentBlockLocation, const FIntVector& ChunkSize, FSurfaceNetsVoxelData& BlockData, TMap<FIntVector, FSurfaceNetsVoxelData>& BlocksMap)
	{
		BlockData.BottomFrontRight = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X + 1, CurrentBlockLocation.Y + 1, CurrentBlockLocation.Z), ChunkSize, BlocksMap);
		BlockData.TopBackRight = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X + 1, CurrentBlockLocation.Y, CurrentBlockLocation.Z + 1), ChunkSize, BlocksMap);
		BlockData.TopFrontRight = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X + 1, CurrentBlockLocation.Y + 1, CurrentBlockLocation.Z + 1), ChunkSize, BlocksMap);
		BlockData.TopFrontLeft = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X, CurrentBlockLocation.Y + 1, CurrentBlockLocation.Z + 1), ChunkSize, BlocksMap);

		BlockData.Left = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X - 1, CurrentBlockLocation.Y, CurrentBlockLocation.Z), ChunkSize, BlocksMap);
		BlockData.Right = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X + 1, CurrentBlockLocation.Y, CurrentBlockLocation.Z), ChunkSize, BlocksMap);
		BlockData.Back = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X, CurrentBlockLocation.Y - 1, CurrentBlockLocation.Z), ChunkSize, BlocksMap);
		BlockData.Front = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X, CurrentBlockLocation.Y + 1, CurrentBlockLocation.Z), ChunkSize, BlocksMap);
		BlockData.Bottom = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X, CurrentBlockLocation.Y, CurrentBlockLocation.Z - 1), ChunkSize, BlocksMap);
		BlockData.Top = __SN_GetBlock(Chunk, FIntVector(CurrentBlockLocation.X, CurrentBlockLocation.Y, CurrentBlockLocation.Z + 1), ChunkSize, BlocksMap);
		
		BlockData.IsSurface = __SN_DetermineIsSurface(BlockData);
	}
	
	static FORCEINLINE void __SN_SumForSmoothedLocation(const FSurfaceNetsVoxelData* CurrentSide, FVector& Sum, int& Count)
	{
		if (CurrentSide && CurrentSide->IsValid()) return;
		Sum += CurrentSide->WorldLocation.BottomBackLeft;
		Count++;
	}
	
	static FORCEINLINE void __SN_FindBlockSmoothLocation(FSurfaceNetsVoxelData& BlockData)
	{
		if (!BlockData.IsSurface) return;

		int Count = 0;
		FVector Sum = FVector::Zero();
		__SN_SumForSmoothedLocation(BlockData.Left, Sum, Count);
		__SN_SumForSmoothedLocation(BlockData.Right, Sum, Count);
		__SN_SumForSmoothedLocation(BlockData.Back, Sum, Count);
		__SN_SumForSmoothedLocation(BlockData.Front, Sum, Count);
		__SN_SumForSmoothedLocation(BlockData.Bottom, Sum, Count);
		__SN_SumForSmoothedLocation(BlockData.Top, Sum, Count);
		
		if (Count == 0) return;
		
		FVector AverageLocation = UGameUtilities::Clamp((Sum / static_cast<float>(Count)), BlockData.WorldLocation.BottomBackLeft, BlockData.WorldLocation.TopFrontRight());
		// FVector Direction = (AverageLocation - BlockData.CalculatedLocation);
		// Direction.Normalize();
		BlockData.CalculatedLocation = FMath::VInterpTo(BlockData.CalculatedLocation, AverageLocation, 1.f, BlockData.Block->FullnessPercentage());
	}
	
	static FORCEINLINE void __SN_AddMeshDataFromBlock(const FSurfaceNetsVoxelData& CurrentBlockData, FVoxelMeshSectionData& MeshData);

private:

	static const FIntVector sc_CornerOffsets[8];
	static const int sc_EdgeConfigurations[256];
	static const int sc_CornerIndexAFromEdge[12];
	static const int sc_CornerIndexBFromEdge[12];
	static const int sc_TrianglePoints[256][16];
};

