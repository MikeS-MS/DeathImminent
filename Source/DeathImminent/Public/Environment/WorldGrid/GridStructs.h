// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "Utilities/GeneralStructs.h"
#include "GridStructs.generated.h"


#define BLOCK_SIZE_DEFAULT 25
#define CHUNK_WIDTH_IN_BLOCKS_DEFAULT 16
#define CHUNK_HEIGHT_IN_BLOCKS_DEFAULT 256
#define WIDTH_IN_BLOCKS_DEFAULT 4096
#define HEIGHT_IN_BLOCKS_DEFAULT 256

USTRUCT(BlueprintType)
struct DEATHIMMINENT_API FGridData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BlockSize{ BLOCK_SIZE_DEFAULT };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntVector ChunksAmount{ WIDTH_IN_BLOCKS_DEFAULT / CHUNK_WIDTH_IN_BLOCKS_DEFAULT, WIDTH_IN_BLOCKS_DEFAULT / CHUNK_WIDTH_IN_BLOCKS_DEFAULT, HEIGHT_IN_BLOCKS_DEFAULT / CHUNK_HEIGHT_IN_BLOCKS_DEFAULT };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FIntVector ChunkSizeInBlocks{ CHUNK_WIDTH_IN_BLOCKS_DEFAULT, CHUNK_WIDTH_IN_BLOCKS_DEFAULT, CHUNK_HEIGHT_IN_BLOCKS_DEFAULT };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntVector SizeInBlocks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ChunkSizeInUnits;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector SizeInUnits;
};

USTRUCT(BlueprintType)
struct DEATHIMMINENT_API FSetBlockOperationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool Completed = false;

	UPROPERTY(BlueprintReadWrite)
	bool WasFull = false;

	UPROPERTY(BlueprintReadWrite)
	int32 BlockFullnessPreChange = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 BlockFullnessPostChange = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 LeftOverFullness = 0;

	UPROPERTY(BlueprintReadWrite)
	FBaseID BlockIDPreChange;

	UPROPERTY(BlueprintReadWrite)
	FBaseID BlockIDPostChange;
};

USTRUCT(BlueprintType)
struct DEATHIMMINENT_API FMultipleBlockEditing
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FIntVector Size { 0, 0, 0 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<int32> Values;
};