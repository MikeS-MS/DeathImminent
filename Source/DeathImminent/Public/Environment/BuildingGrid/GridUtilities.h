// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelDataMeshStructs.h"
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
	static FORCEINLINE int32 GetArrayDesiredSize(const int32& WidthInBlocks, const int32& HeightInBlocks)
	{
		return (WidthInBlocks * WidthInBlocks * HeightInBlocks);
	}

#pragma region Conversion methods
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

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE FBlockID ConvertPackedBlockIDToUnpacked(const int32& PackedBlockID)
	{
		FBlockID BlockID;
		UnpackInt32ToInt16(PackedBlockID, BlockID.Source, BlockID.ID);
		return BlockID;
	}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE int32 ConvertUnpackedBlockIDToPacked(const FBlockID& BlockID)
	{
		return PackInt16ToInt32(BlockID.Source, BlockID.ID);
	}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE FBlockLocations ConvertBlockGridPosToWorldPos(const int& x, const int& y, const int& z, const int& BlockSize, const FVector& Extent)
	{
		const FVector BottomBackLeft = FVector(static_cast<float>(x) * BlockSize, static_cast<float>(y) * BlockSize, static_cast<float>(z) * BlockSize) - Extent;
		return FBlockLocations(BottomBackLeft, BlockSize);
	}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities", DisplayName = "Convert To Array Index")
	static FORCEINLINE int32 ConvertToArrayIndex(const int& x, const int& y, const int& z, const int32& WidthInBlocks)
	{
		return (x + (y * WidthInBlocks) + (z * (WidthInBlocks * WidthInBlocks)));
	}

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE FIntVector ConvertToArrayIndexVector(const FVector& Location, const int32& BlockSizeXY, const int32& BlockSizeZ)
	{
		return FIntVector(FMath::Floor(Location.X) / BlockSizeXY, FMath::Floor(Location.Y) / BlockSizeXY, FMath::Floor(Location.Z) / BlockSizeZ);
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
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "Grid|Utilities")
	static FORCEINLINE bool IsValidBlock(const FBlockID& BlockID)
	{
		return BlockID != FBlockID::Air;
	}

	static FORCEINLINE bool IsValidBlock(const int32& BlockID)
	{
		return BlockID != 0;
	}
};