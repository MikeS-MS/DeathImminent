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

namespace SurfaceNetsData
{
	enum BlockDataCustomSide
	{
		BDCS_Left = 0,
		BDCS_Right,
		BDCS_Back,
		BDCS_Front,
		BDCS_Bottom,
		BDCS_Top,
		BDCS_Last
	};
}

struct FBlockDataForSurfaceNets
{

	FBlockDataForSurfaceNets()
	{
		IsSurface = false;
		IsValid = true;
		WorldLocation = FBlockLocations();
		GridLocation = FIntVector::ZeroValue;
	}

	bool IsSurface;

	bool IsValid;

	FBlockLocations WorldLocation;

	FVector SmoothedLocation;

	FIntVector GridLocation;

	const FBlockDataForSurfaceNets* Left = nullptr;
	const FBlockDataForSurfaceNets* Right = nullptr;
	const FBlockDataForSurfaceNets* Back = nullptr;
	const FBlockDataForSurfaceNets* Front = nullptr;
	const FBlockDataForSurfaceNets* Bottom = nullptr;
	const FBlockDataForSurfaceNets* Top = nullptr;

	const FBlockDataForSurfaceNets* BottomFrontRight = nullptr;
	const FBlockDataForSurfaceNets* TopBackRight = nullptr;
	const FBlockDataForSurfaceNets* TopFrontRight = nullptr;
	const FBlockDataForSurfaceNets* TopFrontLeft = nullptr;
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