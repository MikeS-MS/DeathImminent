// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "VoxelDataMeshStructs.generated.h"

USTRUCT(BlueprintType)
struct FUpdateMeshInformation
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 BlockSize = 25;

	UPROPERTY(BlueprintReadWrite)
	FIntVector ChunkSize;

	UPROPERTY(BlueprintReadWrite)
	FVector UnscaledBoxExtent;
};

USTRUCT(BlueprintType)
struct FVoxelMeshSectionData
{
	GENERATED_BODY();

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


	FVoxelMeshSectionData()
	{

	}

	void Clear()
	{
		Positions.Empty();
		Triangles.Empty();
		UVs.Empty();
		Normals.Empty();
		Tangents.Empty();
	}
};