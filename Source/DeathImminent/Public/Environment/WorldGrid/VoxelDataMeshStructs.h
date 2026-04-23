// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "Environment/Blocks/BlockStructs.h"
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

struct FSurfaceNetsVoxelData
{
	FSurfaceNetsVoxelData(const FBlockLocations& BlockLocations, const FBlock& Block, const FIntVector& GridLocation) : Block(&Block)
	{
		IsSurface = false;
		this -> GridLocation = GridLocation;
		CalculatedLocation = BlockLocations.BottomBackLeft;
		this->WorldLocation = BlockLocations;
	}
	
	bool IsValid() const
	{
		return !Block->IsAir();
	}
	
	bool IsSurface;
	
	FIntVector GridLocation;
	FVector CalculatedLocation;
	const FBlock* Block;
	FBlockLocations WorldLocation;
	
	const FSurfaceNetsVoxelData* Left = nullptr;
	const FSurfaceNetsVoxelData* Right = nullptr;
	const FSurfaceNetsVoxelData* Back = nullptr;
	const FSurfaceNetsVoxelData* Front = nullptr;
	const FSurfaceNetsVoxelData* Bottom = nullptr;
	const FSurfaceNetsVoxelData* Top = nullptr;

	const FSurfaceNetsVoxelData* BottomFrontRight = nullptr;
	const FSurfaceNetsVoxelData* TopBackRight = nullptr;
	const FSurfaceNetsVoxelData* TopFrontRight = nullptr;
	const FSurfaceNetsVoxelData* TopFrontLeft = nullptr;
};