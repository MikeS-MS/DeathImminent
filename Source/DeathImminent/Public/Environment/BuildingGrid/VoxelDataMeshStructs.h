#pragma once

#include "CoreMinimal.h"
#include "Environment/Blocks/BlockStructs.h"
#include "VoxelDataMeshStructs.generated.h"

USTRUCT(BlueprintType)
struct FBlockDataForMarchingCubes
{
	GENERATED_BODY()

	FBlockDataForMarchingCubes()
	{
		CornersStatus.SetNum(8);
		CornerLocations.SetNum(8);
	}

	TArray<int> CornersStatus;
	TArray<FVector> CornerLocations;
};

USTRUCT(BlueprintType)
struct FBlockDataForGreedyMeshGeneration
{
	GENERATED_BODY()

		FBlockDataForGreedyMeshGeneration()
	{
		Block = FBlockID();
		Index = 0;
		RelativeLocation = FIntVector::ZeroValue;
		Center = FVector::Zero();
	}

	UPROPERTY(BlueprintReadWrite)
		FBlockID Block;
	UPROPERTY(BlueprintReadWrite)
		int32 Index;
	UPROPERTY(BlueprintReadWrite)
		FIntVector RelativeLocation;
	UPROPERTY(BlueprintReadWrite)
		FVector Center;
};

USTRUCT(BlueprintType)
struct FGreedyMeshShape
{
	GENERATED_BODY()

		FGreedyMeshShape()
	{
		LengthX = 0;
		LengthY = 0;
		LengthZ = 0;
		Blocks = TArray<FBlockDataForGreedyMeshGeneration>();
	}

	UPROPERTY(BlueprintReadWrite)
		int32 LengthX;

	UPROPERTY(BlueprintReadWrite)
		int32 LengthY;

	UPROPERTY(BlueprintReadWrite)
		int32 LengthZ;

	UPROPERTY(BlueprintReadWrite)
		TArray<FBlockDataForGreedyMeshGeneration> Blocks;
};
