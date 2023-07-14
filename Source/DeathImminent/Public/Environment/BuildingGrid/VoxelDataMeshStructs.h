// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Environment/Blocks/BlockStructs.h"
#include "ProceduralMeshComponent.h"
#include "VoxelDataMeshStructs.generated.h"

USTRUCT(BlueprintType)
struct FBlockStatus
{
	GENERATED_BODY()

	FBlockStatus()
	{
		Status = 0;
		Location = FVector::Zero();
		GridLocation = FIntVector::ZeroValue;
	}

	FBlockStatus(const int& InStatus, const FVector& InLocation, const FIntVector& InGridLocation)
	{
		Status = InStatus;
		Location = InLocation;
		GridLocation = InGridLocation;
	}

	UPROPERTY(BlueprintReadWrite)
	int Status;

	UPROPERTY(BlueprintReadWrite)
	FVector Location;

	UPROPERTY(BlueprintReadWrite)
	FIntVector GridLocation;
};
struct FBlockDataForCustomMeshing;

struct FBlockDataArrayForCustomMeshing
{
	FBlockDataArrayForCustomMeshing(TArray<FBlockDataForCustomMeshing>& BlocksArray, const int& BlocksArrayWidth, const int BlocksArrayHeight) : Blocks(BlocksArray), Width(BlocksArrayWidth), Height(BlocksArrayHeight)
	{
		
	}

	TArray<FBlockDataForCustomMeshing>& Blocks;
	const int Width;
	const int Height;
};

struct FBlockDataForCustomMeshing
{
	FBlockDataForCustomMeshing()
	{
		IsValid = false;
		IsSurface = false;
		NeedsSmoothing = false;
		GridLocation = FIntVector::ZeroValue;
		SmoothedLocation = FVector::Zero();
		Sides.SetNum(6);
		Corners.SetNum(8);
	}

	~FBlockDataForCustomMeshing()
	{
		Sides.Empty();
		Corners.Empty();
	}

	bool IsValid;
	bool IsSurface;
	bool NeedsSmoothing;
	FBlockLocations Bounds;
	FIntVector GridLocation;
	FVector SmoothedLocation;
	TArray<FBlockDataForCustomMeshing*> Sides;
	TArray<FBlockDataForCustomMeshing*> Corners;
};

struct FBlockDataForSurfaceNets
{
	FBlockDataForSurfaceNets()
	{
		IsValid = false;
		IsSurface = false;
		WorldLocation = FBlockLocations();
		GridLocation = FIntVector::ZeroValue;
		Corners.SetNum(8);
		Sides.SetNum(6);
	}

	bool IsValid;
	bool IsSurface;

	FBlockLocations WorldLocation;
	FVector SmoothedLocation;
	FIntVector GridLocation;

	TArray<FBlockDataForSurfaceNets*> Corners;
	TArray<FBlockDataForSurfaceNets*> Sides;
};

struct FBlockDataArrayForSurfaceNets
{
	FBlockDataArrayForSurfaceNets() = delete;

	FBlockDataArrayForSurfaceNets(TArray<FBlockDataForSurfaceNets>& BlocksArray, const int& BlocksArrayWidth, const int& BlocksArrayHeight) : Blocks(BlocksArray), Width(BlocksArrayWidth), Height(BlocksArrayHeight)
	{
	}

	TArray<FBlockDataForSurfaceNets>& Blocks;
	const int Width;
	const int Height;

};

USTRUCT(BlueprintType)
struct FBlockDataForMarchingCubes
{
	GENERATED_BODY()

	FBlockDataForMarchingCubes()
	{
		Corners.SetNum(8);
	}

	UPROPERTY(BlueprintReadWrite)
	TArray<FBlockStatus> Corners;
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

USTRUCT(BlueprintType)
struct FVoxelMeshSectionData
{
	GENERATED_BODY();

	FVoxelMeshSectionData()
	{
		
	}

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Positions;

	UPROPERTY(BlueprintReadWrite)
	TArray<int> Triangles;

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector2D> UVs;

	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> Normals;

	UPROPERTY(BlueprintReadWrite)
	TArray<FProcMeshTangent> Tangents;
};