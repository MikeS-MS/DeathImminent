// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Environment/Blocks/BlockStructs.h"
//#include "DynamicMesh/DynamicMesh3.h"
#include "GridUtilities.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogGridUtilities, Display, Display)

class ABuildingGrid;
class AChunk;

UCLASS()
class DEATHIMMINENT_API UGridUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE int32 GetArrayDesiredSize3D(const int32 Width, const int32 Depth, const int32 Height)
	{
		return (Width * Depth * Height);
	}

	UFUNCTION(BlueprintCallable)
	static int32 PackInt16ToInt32(const int32& FirstInt16, const int32& SecondInt16)
	{
		return ((static_cast<int32>(FirstInt16) << 16) | static_cast<int16>(SecondInt16));
	}

	UFUNCTION(BlueprintCallable)
	static void UnpackInt32ToInt16(const int32& InInt32, int32& OutFirstInt16, int32& OutSecondInt16)
	{
		OutFirstInt16 = static_cast<int16>((InInt32 >> 16) & 0xffff);
		OutSecondInt16 = static_cast<int16>(InInt32 & 0xffff);
	}

	//UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	//static FORCEINLINE FBlockID ConvertPackedBlockIDToUnpacked(const int32& PackedBlockID)
	//{
	//	FBlockID BlockID;
	//	UnpackInt32ToInt16(PackedBlockID, BlockID.Source, BlockID.ID);
	//	return BlockID;
	//}

	//UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	//static FORCEINLINE int32 ConvertUnpackedBlockIDToPacked(const FBlockID& BlockID)
	//{
	//	return PackInt16ToInt32(BlockID.Source, BlockID.ID);
	//}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE FBlockLocations ConvertBlockGridPosToWorldPos(const FIntVector& GridLocation, const int BlockSize, const FVector& Extent)
	{
		const FVector BottomBackLeft = FVector(static_cast<float>(GridLocation.X) * BlockSize, static_cast<float>(GridLocation.Y) * BlockSize, static_cast<float>(GridLocation.Z) * BlockSize) - Extent;
		return FBlockLocations(BottomBackLeft, BlockSize);
	}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities", DisplayName = "Convert To Array Index")
	static FORCEINLINE int32 Convert3DPosTo1DArrayIndex(const FIntVector& GridLocation, const int32 Width, const int32 Depth)
	{
		return (GridLocation.X + (GridLocation.Y * Width) + (GridLocation.Z * (Width * Depth)));
	}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE FIntVector ConvertToArrayIndexVector(const FVector& Location, const FIntVector& BlockSize)
	{
		return FIntVector(static_cast<int>(FMath::Floor(Location.X)) / BlockSize.X, static_cast<int>(FMath::Floor(Location.Y)) / BlockSize.Y, static_cast<int>(FMath::Floor(Location.Z)) / BlockSize.Z);
	}

	/**
	 * Shifts the point by adding the size of the chunk to make it usable by the array since the array is 0 indexed and relative locations can be in the negative XYZ.
	 */
	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE FVector ConvertToArrayUsableVector(const FVector& Location, const FVector& BoxExtent)
	{
		return Location + BoxExtent;
	}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE FVector ConvertToLocalSpaceVector(const FVector& Location, const FTransform& Transform)
	{
		return Transform.InverseTransformPosition(Location);
	}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE FVector ConvertToWorldSpaceFVector(const FVector& Location, const FTransform& Transform)
	{
		return Transform.TransformPosition(Location);
	}
};