// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "Chunk.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GridUtilities.h"
#include "GridMeshUtilities.generated.h"

class AChunk;

UCLASS()
class DEATHIMMINENT_API UGridMeshUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	static void MarchingCubes(AChunk* Chunk, FVoxelMeshSectionData& MeshData, bool& StopEarly);

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

private:

	static const FIntVector sc_CornerOffsets[8];
	static const int sc_EdgeConfigurations[256];
	static const int sc_CornerIndexAFromEdge[12];
	static const int sc_CornerIndexBFromEdge[12];
	static const int sc_TrianglePoints[256][16];
};

