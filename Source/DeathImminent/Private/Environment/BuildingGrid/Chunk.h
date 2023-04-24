// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "RealtimeMeshSimple.h"
#include "RealtimeMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Environment/BuildingGrid/ChunkStructs.h"
#include "Chunk.generated.h"

UCLASS(Blueprintable)
class AChunk : public AActor
{
	GENERATED_BODY()
public:

//~Begin Actor Interface
	AChunk();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
//~End Actor Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SetInformation(int32 width, int32 height, int32 blockSize);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void FillChunkZeroed();

	UFUNCTION(BlueprintCallable, Category = "Test")
	void FillChunkHalf();

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void TryHitBlock(UPARAM(ref) const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void TryPlaceBlock(UPARAM(ref) const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void UpdateMeshes();

protected:

	UFUNCTION(BlueprintCallable, Category = "Utility", meta = (BlueprintProtected))
	FORCEINLINE int32 GetArrayDesiredSize() const
	{
		return (m_WidthInBlocks * m_WidthInBlocks * m_HeightInBlocks);
	}

	UFUNCTION(BlueprintCallable, Category = "Utility|Conversion", meta = (BlueprintProtected))
	FORCEINLINE int32 ConvertToArrayIndex(const FIntVector& Location) const
	{
		return (Location.X + (Location.Y * m_WidthInBlocks) + (Location.Z * (m_WidthInBlocks * m_WidthInBlocks)));
	}

	// Shifts the point by adding the size of the chunk to make it usable by the array since the array is 0 indexed and relative locations can be in the negative XYZ.
	UFUNCTION(BlueprintCallable, Category = "Utility|Conversion")
	FORCEINLINE FVector ConvertToArrayUsableVector(const FVector& Location)
	{
		return Location + OverlapCollision->GetUnscaledBoxExtent();
	}

	UFUNCTION(BlueprintCallable, Category = "Utility|Conversion", meta = (BlueprintProtected))
	FORCEINLINE FIntVector ConvertToArrayIndexVector(const FVector& Location)
	{
		return FIntVector(FMath::Floor(Location.X) / m_BlockSize, FMath::Floor(Location.Y) / m_BlockSize, FMath::Floor(Location.Z) / m_BlockSize);
	}

	UFUNCTION(BlueprintCallable, Category = "Utility|Conversion", meta = (BlueprintProtected))
	FORCEINLINE FVector ConvertToLocalSpaceVector(const FVector& Location)
	{
		return GetTransform().InverseTransformPosition(Location);
	}

	UFUNCTION(BlueprintCallable, Category = "Utility|Conversion", DisplayName = "Convert To World Space", meta = (BlueprintProtected))
	FORCEINLINE FVector ConvertToWorldSpaceFVector(const FVector& Location)
	{
		return GetTransform().TransformPosition(Location);
	}

	UFUNCTION(BlueprintCallable, Category = "Utility|Conversion", DisplayName = "Convert To World Space", meta = (BlueprintProtected))
	FORCEINLINE FVector ConvertToWorldSpaceFIntVector(const FIntVector& Location)
	{
		return GetTransform().TransformPosition(FVector(Location));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Stats")
	int32 GetWidth() const
	{
		return m_WidthInBlocks;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Stats")
	int32 GetHeight() const
	{
		return m_HeightInBlocks;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Stats")
	int32 GetBlockSize() const
	{
		return m_BlockSize;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Stats")
	TArray<int32>& GetBlocks()
	{
		return m_Blocks;
	}

private:
	void _ResetMesh();
	void _GenerateBox(const FBlockShape& Shape, const int32& BlockSize, FRealtimeMeshSimpleMeshData& MeshData);
	void _FindAllShapes(TFunctionRef<void(const FBlockShape&)> GenerateMeshCallback);
	bool _IsBlockValidForShape(TArray<bool>& VisitedBlocks, const FIntVector& Location, FBlockDataForMeshGeneration& OutBlockData);
	TArray<FBlockDataForMeshGeneration> _CheckBlocksForShapeOnY(TArray<bool>& VisitedBlocks, const FIntVector& Location, int32& OutMaxY);
	TArray<FBlockDataForMeshGeneration> _CheckBlocksForShapeOnZ(TArray<bool>& VisitedBlocks, const FIntVector& Location, const int32& MaxY, int32& OutMaxZ);
	TArray<FBlockDataForMeshGeneration> _CheckBlocksForShapeOnX(TArray<bool>& VisitedBlocks, const FIntVector& Location, const int32& MaxY, const int32& MaxZ, int32& OutMaxX);
	FBlockDataForMeshGeneration _FillBlockData(const FIntVector& Location);
	FORCEINLINE int32 _GetBlockIndexFromLocation(const FVector& Location)
	{
		return ConvertToArrayIndex(ConvertToArrayIndexVector(ConvertToArrayUsableVector(ConvertToLocalSpaceVector(Location))));
	}
	FORCEINLINE bool ExecuteFunctionIfIndexValid(const int32& Index, const TFunctionRef<void(const int32&)>& Function) const
	{
		if (!m_Blocks.IsValidIndex(Index))
			return false;

		Function(Index);
		return true;
	}

private:

	UPROPERTY(Replicated)
	int32 m_WidthInBlocks;

	UPROPERTY(Replicated)
	int32 m_HeightInBlocks;

	UPROPERTY(Replicated)
	int32 m_BlockSize;

	TArray<int32> m_Blocks;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* OverlapCollision;

	UPROPERTY(VisibleAnywhere)
	URealtimeMeshComponent* RealtimeMeshComponent;

	UPROPERTY()
	URealtimeMeshSimple* m_Mesh;
};
