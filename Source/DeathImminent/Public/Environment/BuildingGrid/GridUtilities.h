// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelDataMeshStructs.h"
#include "GridUtilities.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogGridUtilities, Display, Display)

UCLASS()
class DEATHIMMINENT_API UGridUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "BuildingGrid|Utility|Conversion")
	static void GetArrayDesiredSize(const int32& WidthInBlocks, const int32& HeightInBlocks, UPARAM(ref) int32& OutResult)
	{
		OutResult = GetArrayDesiredSize(WidthInBlocks, HeightInBlocks);
	}

	UFUNCTION(BlueprintCallable, Category = "BuildingGrid|Utility|Conversion")
	static void ConvertToArrayIndex(const FIntVector& Location, const int32& WidthInBlocks, UPARAM(ref) int32& OutResult)
	{
		OutResult = ConvertToArrayIndex(Location, WidthInBlocks);
	}

	// Shifts the point by adding the size of the chunk to make it usable by the array since the array is 0 indexed and relative locations can be in the negative XYZ.
	UFUNCTION(BlueprintCallable, Category = "BuildingGrid|Utility|Conversion")
	static void ConvertToArrayUsableVector(const FVector& Location, const FVector& BoxExtent, UPARAM(ref) FVector& OutResult)
	{
		OutResult = ConvertToArrayUsableVector(Location, BoxExtent);
	}

	UFUNCTION(BlueprintCallable, Category = "BuildingGrid|Utility|Conversion")
	static void ConvertToArrayIndexVector(const FVector& Location, const int32& BlockSizeXY, const int32& BlockSizeZ, UPARAM(ref) FIntVector& OutResult)
	{
		OutResult = ConvertToArrayIndexVector(Location, BlockSizeXY, BlockSizeZ);
	}

	UFUNCTION(BlueprintCallable, Category = "BuildingGrid|Utility|Conversion")
	static void ConvertToLocalSpaceVector(const FVector& Location, const FTransform& Transform, UPARAM(ref) FVector& OutResult)
	{
		OutResult = ConvertToLocalSpaceVector(Location, Transform);
	}

	UFUNCTION(BlueprintCallable, Category = "BuildingGrid|Utility|Conversion", DisplayName = "Convert To World Space")
	static void ConvertToWorldSpaceFVector(const FVector& Location, const FTransform& Transform, UPARAM(ref) FVector& OutResult)
	{
		OutResult = ConvertToWorldSpaceFVector(Location, Transform);
	}

	UFUNCTION(BlueprintCallable, Category = "BuildingGrid|Utility|Conversion", DisplayName = "Convert To World Space")
	static void ConvertToWorldSpaceFIntVector(const FIntVector& Location, const FTransform& Transform, UPARAM(ref) FVector& OutResult)
	{
		OutResult = ConvertToWorldSpaceFIntVector(Location, Transform);
	}


	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 PackInt16ToInt32(UPARAM(ref) const int32& FirstInt16, UPARAM(ref) const int32& SecondInt16)
	{
		return ((static_cast<int32>(FirstInt16) << 16) | static_cast<int16>(SecondInt16));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void UnpackInt32ToInt16(UPARAM(ref) const int32& InInt32, int32& OutFirstInt16, int32& OutSecondInt16)
	{
		OutFirstInt16 = static_cast<int16>((InInt32 >> 16) & 0xffff);
		OutSecondInt16 = static_cast<int16>(InInt32 & 0xffff);
	}
	
public:

	FORCEINLINE static int32 GetArrayDesiredSize(const int32& WidthInBlocks, const int32& HeightInBlocks)
	{
		return (WidthInBlocks * WidthInBlocks * HeightInBlocks);
	}

#pragma region Conversion methods
	FORCEINLINE static FBlockID ConvertPackedBlockIDToUnpacked(const int32& PackedBlockID)
	{
		FBlockID BlockID;
		UnpackInt32ToInt16(PackedBlockID, BlockID.Source, BlockID.ID);
		return BlockID;
	}
	FORCEINLINE static int32 ConvertUnpackedBlockIDToPacked(const FBlockID& BlockID)
	{
		return PackInt16ToInt32(BlockID.Source, BlockID.ID);
	}
	FORCEINLINE static int32 ConvertToArrayIndex(const FIntVector& GridLocation, const int32& WidthInBlocks)
	{
		return ConvertToArrayIndex(GridLocation.X, GridLocation.Y, GridLocation.Z, WidthInBlocks);
	}
	FORCEINLINE static int32 ConvertToArrayIndex(const int& x, const int& y, const int& z, const int32& WidthInBlocks)
	{
		return (x + (y * WidthInBlocks) + (z * (WidthInBlocks * WidthInBlocks)));
	}
	// Shifts the point by adding the size of the chunk to make it usable by the array since the array is 0 indexed and relative locations can be in the negative XYZ.
	FORCEINLINE static FVector ConvertToArrayUsableVector(const FVector& Location, const FVector& BoxExtent)
	{
		return Location + BoxExtent;
	}
	FORCEINLINE static FIntVector ConvertToArrayIndexVector(const FVector& Location, const int32& BlockSizeXY, const int32& BlockSizeZ)
	{
		return FIntVector(FMath::Floor(Location.X) / BlockSizeXY, FMath::Floor(Location.Y) / BlockSizeXY, FMath::Floor(Location.Z) / BlockSizeZ);
	}
	FORCEINLINE static FVector ConvertToLocalSpaceVector(const FVector& Location, const FTransform& Transform)
	{
		return Transform.InverseTransformPosition(Location);
	}
	FORCEINLINE static FVector ConvertToWorldSpaceFVector(const FVector& Location, const FTransform& Transform)
	{
		return Transform.TransformPosition(Location);
	}
	FORCEINLINE static FVector ConvertToWorldSpaceFIntVector(const FIntVector& Location, const FTransform& Transform)
	{
		return Transform.TransformPosition(FVector(Location));
	}
#pragma endregion

	FORCEINLINE static bool IsValidBlock(const int32& BlockID)
	{
		return BlockID != 0;
	}

	FORCEINLINE static bool IsValidBlock(const FBlockID& BlockID)
	{
		return BlockID != FBlockID::Air();
	}

	FORCEINLINE static int GetConfigurationIndex(const TArray<int>& CornersStatus)
	{
		if (CornersStatus.Num() != 8)
			return 0;

		int ConfigIndex = 0;
		int CurrentConfigNumber = 1;

		for (int i = 0; i < 8; i++)
		{
			const int Status = CornersStatus[i];
			if (Status > 0)
			{
				ConfigIndex |= CurrentConfigNumber;
			}
			CurrentConfigNumber *= 2;
		}

		return ConfigIndex;
	}
	static void AddMeshDataFromBlock(const FBlockDataForMarchingCubes& BlockData, const int& U, const int& V, TArray<FVector>& Positions, TArray<int>& Triangles, TArray<FVector2D>& UVs, TArray<FVector>& Normals, TArray<FVector>& Tangents);

public:

	static const int edgeTable[256];
	static const int cornerIndexAFromEdge[12];
	static const int cornerIndexBFromEdge[12];
	static const int TriangleCombinations[256][16];

};