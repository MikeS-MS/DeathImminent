// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "GridData.generated.h"

#define BLOCK_SIZE_DEFAULT 25
#define CHUNK_WIDTH_IN_BLOCKS_DEFAULT 30
#define CHUNK_HEIGHT_IN_BLOCKS_DEFAULT 30
#define WIDTH_IN_BLOCKS_DEFAULT 4320
#define HEIGHT_IN_BLOCKS_DEFAULT 300

USTRUCT(BlueprintType)
struct FGridData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 BlockSize = BLOCK_SIZE_DEFAULT;

	UPROPERTY(EditAnywhere)
	FIntVector ChunkSizeInBlocks = FIntVector(CHUNK_WIDTH_IN_BLOCKS_DEFAULT, CHUNK_WIDTH_IN_BLOCKS_DEFAULT, CHUNK_HEIGHT_IN_BLOCKS_DEFAULT);

	UPROPERTY(EditAnywhere)
	FIntVector SizeInBlocks = FIntVector(WIDTH_IN_BLOCKS_DEFAULT, WIDTH_IN_BLOCKS_DEFAULT, HEIGHT_IN_BLOCKS_DEFAULT);

	UPROPERTY(VisibleAnywhere)
	FVector ChunkSizeInUnits;

	UPROPERTY(VisibleAnywhere)
	FVector SizeInUnits;

	UPROPERTY(VisibleAnywhere)
	FIntVector ChunksAmount;
};