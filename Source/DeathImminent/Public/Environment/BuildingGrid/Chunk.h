// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "ChunkStructs.h"
#include "GridStructs.h"
#include "Components/BoxComponent.h"
#include "GridUtilities.h"
#include "ProceduralMeshComponent.h"
#include "VoxelDataMeshStructs.h"
#include "Chunk.generated.h"

USTRUCT(BlueprintType)
struct FChunkData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FBlock> Blocks;

	UPROPERTY()
	TArray<AChunk*> ChunksNeededToBeUpdated;
};

class ABuildingGrid;

UCLASS(Blueprintable)
class DEATHIMMINENT_API AChunk : public AActor
{
	GENERATED_BODY()

	friend class ABuildingGrid;
	friend class UGridMeshUtilities;
	friend class UGridUtilities;
	friend class UChunkUpdateQueueComponent;
	friend class FUpdateChunkThread;

public:

	//~Begin Actor Interface
	AChunk();
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End Actor Interface

	/**
	 *  Sets the size of the blocks array and initializes it with air blocks.
	 */
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeBlockArrayWithAir();


	/**
	 * Make sure to provide X, Y and Z in Local Chunk Coordinates (x,y,z >= 0 && x,y,z <= Chunk Width/Depth/Height) and not global grid positions.
	 * Providing anything but Chunk Coordinates will return unexpected results.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FBlockLocations ConvertBlockGridPosToWorldPos(const FIntVector& GridLocation);

	/**
	 * Converts a grid location from local chunk block location to grid location and vice versa.
	 * @ToLocal If true it will convert from global coords to local coords, if false the opposite.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE void ConvertGridLocation(FIntVector& GridLocation, const bool ToLocal = true) const
	{
		if (ToLocal)
			GridLocation -= m__ChunkLocationData.StartGridLocation;
		else
			GridLocation += m__ChunkLocationData.StartGridLocation;
	}

	FORCEINLINE static bool IsLocationInsideUsingGridLocation(const FIntVector& GridLocation, const FIntVector& LowerBound, const FIntVector& UpperBound)
	{
		return ((GridLocation.X >= LowerBound.X && GridLocation.X <= UpperBound.X) && (GridLocation.Y >= LowerBound.Y && GridLocation.Y <= UpperBound.Y)) && (GridLocation.Z >= LowerBound.Z && GridLocation.Z <= UpperBound.Z);
	}

	/**
	 * @param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	 * @return True if the location resides inside the bounds of the current chunk and False if not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	bool IsLocationInsideChunkGridLocation(const FIntVector& GridLocation, const bool GlobalGridCoordinates = true) const;

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	bool IsLocationInsideChunkGridLocationAndBlockExists(const FIntVector& GridLocation, int32& Index, FIntVector& CorrectedPos, const bool GlobalCoordinates = true)
	{
		if (!IsLocationInsideChunkGridLocation(GridLocation, GlobalCoordinates))
			return false;

		CorrectedPos = GridLocation;

		if (GlobalCoordinates)
			ConvertGridLocation(CorrectedPos, GlobalCoordinates);

		Index = _GetBlockIndexFromGridLocation(CorrectedPos);

		if (!m__ChunkData.Blocks.IsValidIndex(Index))
			return false;

		return true;
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE ABuildingGrid* GetContainingGrid()
	{
		return m__ContainingGrid;
	}

protected:

	void _SetInformation(ABuildingGrid* ContainingGrid, const FChunkLocationData& ChunkLocationData);

	UFUNCTION(Category = "Operations", BlueprintCallable, meta = (BluepritProtected))
	void _SetShouldUpdate();

	/**
	 * Loops through all the blocks in the current chunk and creates a mesh to represent it.
	 */
	UFUNCTION(Category = "Operations", BlueprintCallable, meta = (BluepritProtected))
	void _UpdateMesh(const FVoxelMeshSectionData& MeshData);

	/**
	 * Clears all mesh buffers.
	 */
	UFUNCTION(Category = "Operations", BlueprintCallable, meta = (BlueprintProtected))
	void _ResetMesh();

	/**
	 * Updates the mesh of all the surrounding chunks for the affected block.
	 * If Location is not on any of the borders, it will not update any surrounding chunks.
	 */
	UFUNCTION(DisplayName = "UpdateMeshOfSurroundingChunks", Category = "Operations", BlueprintCallable, meta = (BlueprintProtected))
	void _UpdateMeshOfSurroundingChunks(const FIntVector& GridLocation, TArray<AChunk*>& ChunksToBeUpdated);

	/**
	 * Returns the Block at that location taking into account if the coordinates are local to directly access the block array instead of making a call to the grid system.
	 * @return The BlockID and Value at the target location, if Location is invalid it will return an invalid block.
	 */
	//FBlock& _GetBlockAtGridLocationOptimizedForLocal(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false);

	UFUNCTION(DisplayName = "GetBlockIndexFromGridLocation", Category = "Operations", BlueprintCallable, meta = (BlueprintProtected))
	int32 _GetBlockIndexFromGridLocation(const FIntVector& GridLocation) const;

	UFUNCTION(DisplayName = "GetBlockAtGridLocation", Category = "Operations", BlueprintCallable, meta = (BlueprintProtected))
	FBlock _GetBlockAtGridLocation(const FIntVector& GridLocation, bool& Result, const bool GlobalCoordinates = true) const;

private:

	UFUNCTION()
	void OnRep_ChunkData();

	TArray<FBlock> __CopyRelevantBlocksForUpdate() const;
	FSetBlockOperationResult __SetBlockAtGridLocation(const FIntVector& GridLocation, 
													  const FBaseID& BlockID, 
													  const double Fullness = 0.0, 
													  const bool GlobalCoordinates = true, 
													  const bool Increments = true, 
													  const bool Force = false, 
													  const bool ShouldUpdate = true);

private:

	UPROPERTY(DisplayName = "ChunkLocationData", Category = "Data", VisibleInstanceOnly, Replicated)
	FChunkLocationData m__ChunkLocationData;

	UPROPERTY(ReplicatedUsing = OnRep_ChunkData)
	FChunkData m__ChunkData;

	UPROPERTY(Replicated)
	ABuildingGrid* m__ContainingGrid;

	UPROPERTY(VisibleInstanceOnly)
	UBoxComponent* m__OverlapCollision;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* m__ProceduralMeshComponent;

	// TODO: To be removed, only a testing material.
	UPROPERTY(VisibleInstanceOnly)
	UMaterialInstance* m__GridMaterial;
};
