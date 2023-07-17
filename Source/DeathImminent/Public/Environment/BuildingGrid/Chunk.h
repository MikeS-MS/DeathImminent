// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Environment/BuildingGrid/VoxelDataMeshStructs.h"
#include "GridUtilities.h"
#include "ProceduralMeshComponent.h"
#include "Chunk.generated.h"

class ABuildingGrid;

UCLASS(Blueprintable)
class AChunk : public AActor
{
	GENERATED_BODY()

	friend class ABuildingGrid;
	friend class UGridMeshUtilities;
	friend class UGridUtilities;

public:

	//~Begin Actor Interface
	AChunk();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End Actor Interface

#pragma region Setup
	/**
	 *	@param ChunkPosInGrid This is the location of the chunk in the chunk array inside BuildingGrid.
	 *	@param ChunkIndexInGrid This is the index of the chunk in the chunk array inside BuildingGrid.
	 *	@param StartX, StartY, StartZ This is the index of the first block in this chunk in grid coordinates (not local).
	 */
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SetInformation(ABuildingGrid* ContainingGrid, const FIntVector& ChunkPosInGrid, const int32& ChunkIndexInGrid, const int32& StartX, const int32& StartY, const int32& StartZ);

	/**
	 *  Sets the size of the blocks array and initializes it with air blocks.
	 */
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void InitializeBlockArrayWithAir();
#pragma endregion Public

#pragma region Operations
	/**
	 * Loops through all the blocks in the current chunk and creates a mesh to represent it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Operations")
	void UpdateMesh();

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void TryHitBlock(UPARAM(ref) const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Operations")
	bool TryPlaceBlock(UPARAM(ref) const FVector& Location, const FBlockID& BlockID, const bool& ShouldUpdateMeshes = true);

	/**
	 * Sets a block type at the given grid location.
	 * @param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	 */
	UFUNCTION(BlueprintCallable, Category = "Operations")
	bool SetBlock(const FBlockID& BlockID, const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = true, const bool ShouldUpdateMesh = true);
#pragma endregion Public

#pragma region Utilities
	/**
	 * Make sure to provide X, Y and Z in Local Chunk Coordinates (x,y,z >= 0 && x,y,z <= Chunk Width/Depth/Height) and not global grid positions.
	 * Providing anything but Chunk Coordinates will return unexpected results.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE FBlockLocations ConvertBlockGridPosToWorldPos(const int& x, const int& y, const int& z)
	{
		return UGridUtilities::ConvertBlockGridPosToWorldPos(x, y, z, m__BlockSize, m__OverlapCollision->GetUnscaledBoxExtent());
	}

	/**
	 * Converts a grid location from local chunk block location to grid location and vice versa.
	 * @ToLocal If true it will convert from global coords to local coords, if false the opposite.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE void ConvertGridLocation(int& x, int& y, int& z, const bool& ToLocal = true) const
	{
		if (ToLocal)
		{
			x -= m__StartGridLocation.X;
			y -= m__StartGridLocation.Y;
			z -= m__StartGridLocation.Z;
		}
		else
		{
			x += m__StartGridLocation.X;
			y += m__StartGridLocation.Y;
			z += m__StartGridLocation.Z;
		}
	}

	/**
	 * Returns the Block at that world location.
	 * @return The BlockID and Value at the target location, if Location is invalid it will return an invalid block.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE Block* GetBlockAtWorldLocation(const FVector& Location)
	{
		if (!IsLocationInsideChunkInUnits(Location))
			return nullptr;

		FVector LocalSpaceLocation;
		FVector ArrayUsableLocation;
		FIntVector GridLocation;
		return &m__Blocks[_GetBlockIndexFromWorldLocation(Location, LocalSpaceLocation, ArrayUsableLocation, GridLocation)];
	}

	/**
	 * Returns the Block at that location, taking into account if the coordinates given should be converted to local or kept as is because they are already local.
	 * @param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	 * @return The BlockID and Value at the target location, if Location is invalid it will return an invalid block.
	 */
	FORCEINLINE Block* GetBlockAtGridLocation(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false)
	{
		if (!IsLocationInsideChunkInBlocks(x, y, z, GlobalGridCoordinates))
			return nullptr;

		int newX = x;
		int newY = y;
		int newZ = z;

		if (GlobalGridCoordinates)
			ConvertGridLocation(newX, newY, newZ);

		return &m__Blocks[UGridUtilities::ConvertToArrayIndex(newX, newY, newZ, m__WidthInBlocks)];
	}

	/**
	 * @return True if the location resides inside the bounds of the current chunk and False if not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE bool IsLocationInsideChunkInUnits(const FVector& Location) const
	{
		const FVector& ChunkLocation = GetTransform().GetLocation();
		const FVector& Extent = m__OverlapCollision->GetUnscaledBoxExtent();
		return ((Location.X >= (ChunkLocation.X - Extent.X) && Location.X <= (ChunkLocation.X + Extent.X)) && (Location.Y >= (ChunkLocation.Y - Extent.Y) && Location.Y <= (ChunkLocation.Y + Extent.Y))) && (Location.Z >= (ChunkLocation.Z - Extent.Z) && Location.Z <= (ChunkLocation.Z + Extent.Z));
	}

	/**
	 * @param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	 * @return True if the location resides inside the bounds of the current chunk and False if not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE bool IsLocationInsideChunkInBlocks(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false) const
	{
		if (GlobalGridCoordinates)
			return ((x >= m__StartGridLocation.X && x <= m__EndGridLocation.X) && (y >= m__StartGridLocation.Y && y <= m__EndGridLocation.Y)) && (z >= m__StartGridLocation.Z && z <= m__EndGridLocation.Z);

		return ((x >= 0 && x < m__WidthInBlocks) && (y >= 0 && y < m__WidthInBlocks)) && (z >= 0 && z < m__HeightInBlocks);
	}
#pragma endregion Public

#pragma region Blueprint Getters
	UFUNCTION(BlueprintGetter, Category = "Stats")
	const FIntVector& GetChunkPosInGrid() const
	{
		return m__ChunkPosInGrid;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetChunkIndexInGrid() const
	{
		return m__ChunkIndexInGrid;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const FIntVector& GetStartGridLocation() const
	{
		return m__StartGridLocation;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const FIntVector& GetEndGridLocation() const
	{
		return m__EndGridLocation;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetWidthInBlocks() const
	{
		return m__WidthInBlocks;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetHeightInBlocks() const
	{
		return m__HeightInBlocks;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetBlockSize() const
	{
		return m__BlockSize;
	}
#pragma endregion Public

protected:

#pragma region Operations
	/**
	 * Clears all mesh buffers.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Operations")
	void _ResetMesh();

	/**
	 * Updates the mesh of all the surrounding chunks for the affected block.
	 * If Location is not on any of the borders, it will not update any surrounding chunks.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Operations")
	void _UpdateMeshOfSurroundingChunks(const int& x, const int& y, const int& z);
#pragma endregion Protected

#pragma region Utilities
	/**
	 * Returns the Block at that location taking into account if the coordinates are local to directly access the block array instead of making a call to the grid system.
	 * @param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	 * @return The BlockID and Value at the target location, if Location is invalid it will return an invalid block.
	 */
	Block* _GetBlockAtGridLocationOptimizedForLocal(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Operations", DisplayName = "Get Block Index From World Location")
	FORCEINLINE int32 _GetBlockIndexFromWorldLocation(const FVector& Location, FVector& OutLocalSpaceLocation, FVector& OutArrayUsableLocation, FIntVector& OutGridLocation) const
	{
		OutLocalSpaceLocation = UGridUtilities::ConvertToLocalSpaceVector(Location, GetTransform());
		OutArrayUsableLocation = UGridUtilities::ConvertToArrayUsableVector(OutLocalSpaceLocation, m__OverlapCollision->GetUnscaledBoxExtent());
		OutGridLocation = UGridUtilities::ConvertToArrayIndexVector(OutArrayUsableLocation, m__BlockSize, m__BlockSize);
		return UGridUtilities::ConvertToArrayIndex(OutGridLocation.X, OutGridLocation.Y, OutGridLocation.Z, m__WidthInBlocks);
	}
#pragma endregion Protected

#pragma region Blueprint Getters
	//UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Data")
	//TArray<int32>& GetBlocks()
	//{
	//	return m_Blocks;
	//}
#pragma endregion Protected

private:

#pragma region Operations
	FORCEINLINE void __ResetPositions()
	{
		const FTransform& Transform = GetTransform();
		for(int x = 0; x < m__WidthInBlocks; x++)
		{
			for (int y = 0; y < m__WidthInBlocks; y++)
			{
				for (int z = 0; z < m__HeightInBlocks; z++)
				{
					Block& Block = m__Blocks[UGridUtilities::ConvertToArrayIndex(x, y, z, m__WidthInBlocks)];
					const FVector Center = ConvertBlockGridPosToWorldPos(x, y, z).Center();
					const FVector InitialLocation = UGridUtilities::ConvertToWorldSpaceFVector(Center, Transform);
					Block.X = InitialLocation.X;
					Block.Y = InitialLocation.Y;
					Block.Z = InitialLocation.Z;
				}
			}
		}
	}
	FORCEINLINE void __SetBlockStatus(const int& x, const int& y, const int& z, const bool& ShouldUpdate = false);
#pragma endregion Private

#pragma region Utilities
	FORCEINLINE bool __ExecuteFunctionIfIndexValid(const int32& ArrayIndex, const TFunctionRef<void(const int32&)>& Function) const
	{
		if (!m__Blocks.IsValidIndex(ArrayIndex))
			return false;

		Function(ArrayIndex);
		return true;
	}
#pragma endregion Private

protected:

	UPROPERTY(BlueprintGetter = GetChunkPosInGrid, DisplayName = "Chunk Position In Grid", Category = "Stats")
	FIntVector m__ChunkPosInGrid;

	UPROPERTY(BlueprintGetter = GetChunkIndexInGrid, DisplayName = "Start X", Category = "Stats")
	int32 m__ChunkIndexInGrid;

	UPROPERTY(BlueprintGetter = GetStartGridLocation, DisplayName = "Start Z", Category = "Stats")
	FIntVector m__StartGridLocation;

	UPROPERTY(BlueprintGetter = GetEndGridLocation, DisplayName = "Start Z", Category = "Stats")
	FIntVector m__EndGridLocation;

	UPROPERTY(BlueprintGetter = GetWidthInBlocks, DisplayName = "Width in Blocks", Category = "Stats")
	int32 m__WidthInBlocks;

	UPROPERTY(BlueprintGetter = GetHeightInBlocks, DisplayName = "Height in Blocks", Category = "Stats")
	int32 m__HeightInBlocks;

	// In units.
	UPROPERTY(BlueprintGetter = GetBlockSize, DisplayName = "Block Size", Category = "Stats")
	int32 m__BlockSize;

	TArray<Block> m__Blocks;

	UPROPERTY()
	ABuildingGrid* m__ContainingGrid;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* m__OverlapCollision;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* m__ProceduralMeshComponent;

	UPROPERTY()
	UMaterialInstance* m__GridMaterial;
};