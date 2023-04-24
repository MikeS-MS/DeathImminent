#pragma once

#include "CoreMinimal.h"
#include "Environment/Blocks/BlockStructs.h"
#include "ChunkStructs.generated.h"

USTRUCT(BlueprintType)
struct FBlockDataForMeshGeneration
{
	GENERATED_BODY()

	FBlockDataForMeshGeneration()
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
struct FBlockShape
{
	GENERATED_BODY()

	FBlockShape()
	{
		LengthX = 0;
		LengthY = 0;
		LengthZ = 0;
		Blocks = TArray<FBlockDataForMeshGeneration>();
	}

	UPROPERTY(BlueprintReadWrite)
	int32 LengthX;

	UPROPERTY(BlueprintReadWrite)
	int32 LengthY;

	UPROPERTY(BlueprintReadWrite)
	int32 LengthZ;

	UPROPERTY(BlueprintReadWrite)
	TArray<FBlockDataForMeshGeneration> Blocks;
};
