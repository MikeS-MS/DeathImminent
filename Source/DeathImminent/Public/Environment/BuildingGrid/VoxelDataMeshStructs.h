// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "VoxelDataMeshStructs.generated.h"

USTRUCT(BlueprintType)
struct FUpdateMeshInformation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 BlockSize;

	UPROPERTY(BlueprintReadWrite)
	FIntVector ChunkSize;

	UPROPERTY(BlueprintReadWrite)
	FVector UnscaledBoxExtent;
};

USTRUCT(BlueprintType)
struct FBlockStatus
{
	GENERATED_BODY()

	FBlockStatus()
	{
		Status = 0;
		Location = FVector::Zero();
		GridLocation = FIntVector::ZeroValue;
		Value = 0;
	}

	FBlockStatus(const int& InStatus, const float& InValue, const FVector& InLocation, const FIntVector& InGridLocation)
	{
		Status = InStatus;
		Location = InLocation;
		GridLocation = InGridLocation;
		Value = InValue;
	}

	UPROPERTY(BlueprintReadWrite)
	float Value;

	UPROPERTY(BlueprintReadWrite)
	int Status;

	UPROPERTY(BlueprintReadWrite)
	FVector Location;

	UPROPERTY(BlueprintReadWrite)
	FIntVector GridLocation;
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