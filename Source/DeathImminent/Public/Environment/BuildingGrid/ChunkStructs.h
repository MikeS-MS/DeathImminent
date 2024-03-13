#pragma once

#include "CoreMinimal.h"
#include "ChunkStructs.generated.h"

USTRUCT(BlueprintType)
struct FChunkLocationData
{
	GENERATED_BODY()

	/**
	* ChunkIndexInGrid This is the index of the chunk in the chunk array inside BuildingGrid.
	*/
	UPROPERTY(Category = "Data", VisibleAnywhere, BlueprintReadOnly)
	int32 IndexInGrid;

	/**
	 * This is the location of the chunk in the chunk array inside BuildingGrid.
	 */
	UPROPERTY(Category = "Data", VisibleAnywhere, BlueprintReadOnly)
	FIntVector PositionInGrid;

	/**
	 * This is the location of the first block in this chunk in grid coordinates (not local).
	 */

	UPROPERTY(Category = "Data", VisibleAnywhere, BlueprintReadOnly)
	FIntVector StartGridLocation;

	/**
	* This is the location of the last block in this chunk in grid coordinates (not local).
	*/
	UPROPERTY(Category = "Data", VisibleAnywhere, BlueprintReadOnly)
	FIntVector EndGridLocation;
};