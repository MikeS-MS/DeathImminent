// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "RealtimeMeshSimple.h"
#include "RealtimeMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Environment/BuildingGrid/VoxelDataMeshStructs.h"
#include "GridUtilities.h"
#include "RealtimeMeshLibrary.h"
#include "Chunk.generated.h"

class ABuildingGrid;

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

#pragma region Setup
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void SetInformation(ABuildingGrid* ContainingGrid, const FIntVector& ChunkPosInGrid, const int32& ChunkIndexInGrid, const int32& StartX, const int32& StartY, const int32& StartZ);

	UFUNCTION(BlueprintCallable, Category = "Setup")
	void FillChunkZeroed();
#pragma endregion Public

#pragma region Operations
	UFUNCTION(BlueprintCallable, Category = "Operations")
	void FillChunk(const int32& FillX = -1, const int32& FillY = -1, const int32& FillZ = -1, const FBlockID& BlockID = FBlockID());
#pragma endregion Public

#pragma region Chunk Interface methods
	UFUNCTION(BlueprintCallable, Category = "Operations")
	void TryHitBlock(UPARAM(ref) const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Operations")
	bool TryPlaceBlock(UPARAM(ref) const FVector& Location, const FBlockID& BlockID, const bool& ShouldUpdateMeshes = true);

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void UpdateMesh();
#pragma endregion Public

#pragma region Blueprint Getters
	UFUNCTION(BlueprintGetter, Category = "Stats")
	const FIntVector& GetChunkPosInGrid() const
	{
		return m_ChunkPosInGrid;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetChunkIndexInGrid() const
	{
		return m_ChunkIndexInGrid;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetStartX() const
	{
		return m_StartX;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetStartY() const
	{
		return m_StartY;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetStartZ() const
	{
		return m_StartZ;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetEndX() const
	{
		return m_EndX;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetEndY() const
	{
		return m_EndY;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetEndZ() const
	{
		return m_EndZ;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetWidthInBlocks() const
	{
		return m_WidthInBlocks;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetHeightInBlocks() const
	{
		return m_HeightInBlocks;
	}

	UFUNCTION(BlueprintGetter, Category = "Stats")
	const int32& GetBlockSize() const
	{
		return m_BlockSize;
	}
#pragma endregion Public

#pragma region Utilities
	//@param ToLocal if this is set to true it will convert from grid coord to local array coord, if false it will be from local array coord to grid coord.
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	void ConvertGridLocation(UPARAM(ref) FIntVector& Location, const bool& ToLocal = true)
	{
		ConvertGridLocation(Location.X, Location.Y, Location.Z, ToLocal);
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	void IsLocationInsideChunkInUnits(const FVector& Location, bool& OutResult) const
	{
		OutResult = IsLocationInsideChunkInUnits(Location);
	}

	//@param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	UFUNCTION(BlueprintCallable, DisplayName = "Is Location Inside Chunk In Blocks", Category = "Utilities")
	void IsLocationInsideChunkInBlocks(bool& OutResult, const FIntVector& Location, const bool& GlobalGridCoordinates = false) const
	{
		OutResult = IsLocationInsideChunkInBlocks(Location, GlobalGridCoordinates);
	}

	//@param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	UFUNCTION(BlueprintCallable, DisplayName = "Is Location Inside Chunk In Blocks", Category = "Utilities")
	void IsLocationInsideChunkInBlocksXYZ(bool& OutResult, const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false) const
	{
		OutResult = IsLocationInsideChunkInBlocks(x, y, z, GlobalGridCoordinates);
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	void GetBlockAtWorldLocation(const FVector& Location, FBlockID& OutBlockID)
	{
		OutBlockID = GetBlockAtWorldLocation(Location);
	}

	UFUNCTION(BlueprintCallable, DisplayName = "Get Block At Grid Location", Category = "Utilities")
	void GetBlockAtGridLocation(FBlockID& OutBlockID, const FIntVector& Location, const bool& GlobalGridCoordinates = false) const
	{
		OutBlockID = GetBlockAtGridLocation(Location, GlobalGridCoordinates);
	}

	UFUNCTION(BlueprintCallable, DisplayName = "Get Block At Grid Location", Category = "Utilities")
	void GetBlockAtGridLocationXYZ(FBlockID& OutBlockID, const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false) const
	{
		OutBlockID = GetBlockAtGridLocation(x, y, z, GlobalGridCoordinates);
	}

	//@param ToLocal if this is set to true it will convert from grid coord to local array coord, if false it will be from local array coord to grid coord.
	FORCEINLINE void ConvertGridLocation(int& x, int& y, int& z, const bool& ToLocal = true) const
	{
		if (ToLocal)
		{
			x -= m_StartX;
			y -= m_StartY;
			z -= m_StartZ;
		}
		else
		{
			x += m_StartX;
			y += m_StartY;
			z += m_StartZ;
		}
	}

	FORCEINLINE bool IsLocationInsideChunkInUnits(const FVector& Location) const
	{
		const FVector& ChunkLocation = GetTransform().GetLocation();
		const FVector& Extent = m_OverlapCollision->GetUnscaledBoxExtent();
		return ((Location.X >= (ChunkLocation.X - Extent.X) && Location.X <= (ChunkLocation.X + Extent.X)) && (Location.Y >= (ChunkLocation.Y - Extent.Y) && Location.Y <= (ChunkLocation.Y + Extent.Y))) && (Location.Z >= (ChunkLocation.Z - Extent.Z) && Location.Z <= (ChunkLocation.Z + Extent.Z));
	}
	//@param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	FORCEINLINE bool IsLocationInsideChunkInBlocks(const FIntVector& Location, const bool& GlobalGridCoordinates = false) const
	{
		return IsLocationInsideChunkInBlocks(Location.X, Location.Y, Location.Z, GlobalGridCoordinates);
	}
	//@param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	FORCEINLINE bool IsLocationInsideChunkInBlocks(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false) const
	{
		if (GlobalGridCoordinates)
			return ((x >= m_StartX && x <= m_EndX) && (y >= m_StartY && y <= m_EndY)) && (z >= m_StartZ && z <= m_EndZ);

		return ((x >= 0 && x < m_WidthInBlocks) && (y >= 0 && y < m_WidthInBlocks)) && (z >= 0 && z < m_HeightInBlocks);
	}

	FORCEINLINE FBlockID GetBlockAtWorldLocation(const FVector& Location) const
	{
		if (!IsLocationInsideChunkInUnits(Location))
			return FBlockID::Invalid();

		FVector LocalSpaceLocation;
		FVector ArrayUsableLocation;
		FIntVector GridLocation;
		const int32& BlockIDMerged = m_Blocks[_GetBlockIndexFromLocation(Location, LocalSpaceLocation, ArrayUsableLocation, GridLocation)];

		return UGridUtilities::ConvertPackedBlockIDToUnpacked(BlockIDMerged);
	}
	//@param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	FORCEINLINE FBlockID GetBlockAtGridLocation(const FIntVector& Location, const bool& GlobalGridCoordinates = false) const
	{
		return GetBlockAtGridLocation(Location.X, Location.Y, Location.Z, GlobalGridCoordinates);
	}
	//@param GlobalGridCoordinates if this is true the function expects the location to range from the grid's first to the grid's last index, if not true, then it ranges from the current chunk's first and last index.
	FORCEINLINE FBlockID GetBlockAtGridLocation(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false) const
	{
		if (!IsLocationInsideChunkInBlocks(x, y, z, GlobalGridCoordinates))
			return FBlockID::Invalid();

		int newX = x;
		int newY = y;
		int newZ = z;

		if (GlobalGridCoordinates)
			ConvertGridLocation(newX, newY, newZ);

		return UGridUtilities::ConvertPackedBlockIDToUnpacked(m_Blocks[UGridUtilities::ConvertToArrayIndex(newX, newY, newZ, m_WidthInBlocks)]);
	}
#pragma endregion Public

protected:

#pragma region Blueprint Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected), Category = "Data")
	TArray<int32>& GetBlocks()
	{
		return m_Blocks;
	}
#pragma endregion Protected

#pragma region Operations
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Operations")
	void _ResetMesh();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected), Category = "Operations")
	void _UpdateMeshSurroundingChunks(const int& x, const int& y, const int& z);
#pragma endregion Protected

private:

#pragma region Utilities
	FORCEINLINE bool _ExecuteFunctionIfIndexValid(const int32& Index, const TFunctionRef<void(const int32&)>& Function) const
	{
		if (!m_Blocks.IsValidIndex(Index))
			return false;

		Function(Index);
		return true;
	}
	// Checks if the position is inside the current chunk and if it is, it will return it directly from the current chunk instead of doing a call to the grid.
	FORCEINLINE int32 _GetBlockIndexFromLocation(const FVector& Location, FVector& OutLocalSpaceLocation, FVector& OutArrayUsableLocation, FIntVector& OutGridLocation) const
	{
		OutLocalSpaceLocation = UGridUtilities::ConvertToLocalSpaceVector(Location, GetTransform());
		OutArrayUsableLocation = UGridUtilities::ConvertToArrayUsableVector(OutLocalSpaceLocation, m_OverlapCollision->GetUnscaledBoxExtent());
		OutGridLocation = UGridUtilities::ConvertToArrayIndexVector(OutArrayUsableLocation, m_BlockSize, m_BlockSize);
		return UGridUtilities::ConvertToArrayIndex(OutGridLocation, m_WidthInBlocks);
	}
	FORCEINLINE FBlockID _GetBlockAtLocationOptimizedForLocal(const FIntVector& Location, const bool& GlobalGridCoordinates = false) const
	{
		return _GetBlockAtLocationOptimizedForLocal(Location.X, Location.Y, Location.Z, GlobalGridCoordinates);
	}
	FORCEINLINE FBlockID _GetBlockAtLocationOptimizedForLocal(const int& x, const int& y, const int& z, const bool& GlobalGridCoordinates = false) const;
#pragma endregion Private

#pragma region Greedy Meshing Algorithm
	void _FindAllShapes(TFunctionRef<void(const FGreedyMeshShape&)> GenerateMeshCallback);
	FORCEINLINE bool _IsBlockValidForShape(TArray<bool>& VisitedBlocks, const FIntVector& Location, FBlockDataForGreedyMeshGeneration& OutBlockData)
	{
		const int32 BlockIndex = UGridUtilities::ConvertToArrayIndex(Location, m_WidthInBlocks);

		if (VisitedBlocks[BlockIndex])
			return false;

		const int32& BlockID = m_Blocks[BlockIndex];

		if (!UGridUtilities::IsValidBlock(BlockID))
			return false;

		OutBlockData = _GetBlockDataForGreedyMeshing(Location);

		return true;
	}
	FORCEINLINE TArray<FBlockDataForGreedyMeshGeneration> _CheckBlocksForShapeOnY(TArray<bool>& VisitedBlocks, const FIntVector& Location, int32& OutMaxY)
	{
		TArray<FBlockDataForGreedyMeshGeneration> Blocks;

		// Invalid index
		if (Location.Y > m_WidthInBlocks - 1)
			return Blocks;

		for (int y = Location.Y; y < m_WidthInBlocks; y++)
		{
			if (FIntVector(Location.X, y, Location.Z) == Location)
				continue;

			FBlockDataForGreedyMeshGeneration BlockData;
			if (!_IsBlockValidForShape(VisitedBlocks, FIntVector(Location.X, y, Location.Z), BlockData))
				return Blocks;

			Blocks.Add(BlockData);
			VisitedBlocks[BlockData.Index] = true;
			OutMaxY = y;
		}

		return Blocks;
	}
	FORCEINLINE TArray<FBlockDataForGreedyMeshGeneration> _CheckBlocksForShapeOnZ(TArray<bool>& VisitedBlocks, const FIntVector& Location, const int32& MaxY, int32& OutMaxZ)
	{
		TArray<FBlockDataForGreedyMeshGeneration> Blocks;

		int32 MaxZ = m_HeightInBlocks - 1;

		// Invalid index
		if (Location.Z > MaxZ)
			return Blocks;

		// Get the lowest average index for Z to see if we can combine
		for (int y = Location.Y; y <= MaxY; y++)
		{
			for (int z = Location.Z; z < m_HeightInBlocks; z++)
			{
				if ((y >= Location.Y && y <= MaxY) && z == Location.Z)
					continue;

				FBlockDataForGreedyMeshGeneration BlockData;
				if (!_IsBlockValidForShape(VisitedBlocks, FIntVector(Location.X, y, z), BlockData))
				{
					if (z == Location.Z + 1)
						return Blocks;

					if (z - 1 < MaxZ)
						MaxZ = z - 1;

					break;
				}
			}
		}

		OutMaxZ = MaxZ;

		// Add all blocks from the given span
		for (int y = Location.Y; y <= MaxY; y++)
		{
			for (int z = Location.Z; z <= MaxZ; z++)
			{
				FBlockDataForGreedyMeshGeneration BlockData = _GetBlockDataForGreedyMeshing(FIntVector(Location.X, y, z));
				Blocks.Add(BlockData);
				VisitedBlocks[BlockData.Index] = true;
			}
		}

		return Blocks;
	}
	FORCEINLINE TArray<FBlockDataForGreedyMeshGeneration> _CheckBlocksForShapeOnX(TArray<bool>& VisitedBlocks, const FIntVector& Location, const int32& MaxY, const int32& MaxZ, int32& OutMaxX)
	{
		TArray<FBlockDataForGreedyMeshGeneration> Blocks;

		int32 MaxX = m_WidthInBlocks - 1;

		// Invalid index
		if (Location.X > MaxX)
			return Blocks;

		// fix checks for X axis
		for (int y = Location.Y; y <= MaxY; y++)
		{
			for (int z = Location.Z; z <= MaxZ; z++)
			{
				for (int x = Location.X; x < m_WidthInBlocks; x++)
				{
					// Skip initial block as we have already added that one.
					if (x == Location.X && ((y >= Location.Y && y <= MaxY) && (z >= Location.Z && z <= MaxZ)))
						continue;

					FBlockDataForGreedyMeshGeneration BlockData;
					if (!_IsBlockValidForShape(VisitedBlocks, FIntVector(x, y, z), BlockData))
					{
						if (x == Location.X + 1)
							return Blocks;

						if (x - 1 < MaxX)
							MaxX = x - 1;
						break;
					}
				}
			}
		}

		OutMaxX = MaxX;

		for (int y = Location.Y; y <= MaxY; y++)
		{
			for (int z = Location.Z; z <= MaxZ; z++)
			{
				for (int x = Location.X; x <= MaxX; x++)
				{
					FBlockDataForGreedyMeshGeneration BlockData = _GetBlockDataForGreedyMeshing(FIntVector(x, y, z));
					Blocks.Add(BlockData);
					VisitedBlocks[BlockData.Index] = true;
				}
			}
		}

		return Blocks;
	}
	FORCEINLINE void _GenerateBox(const FGreedyMeshShape& Shape, const int32& BlockSize, FRealtimeMeshSimpleMeshData& MeshData) const
	{
		const FBlockDataForGreedyMeshGeneration FirstBlock = Shape.Blocks[0];
		const FBlockDataForGreedyMeshGeneration LastBlock = Shape.Blocks[Shape.Blocks.Num() - 1];

		const float HalfBlockSize = static_cast<float>(BlockSize) / 2.f;

		const float XMin = FirstBlock.Center.X - HalfBlockSize;
		const float XMax = LastBlock.Center.X + HalfBlockSize;
		const float LengthX = FMath::Abs(XMax - XMin);
		const float HalfLengthX = LengthX / 2;

		const float YMin = FirstBlock.Center.Y - HalfBlockSize;
		const float YMax = LastBlock.Center.Y + HalfBlockSize;
		const float LengthY = FMath::Abs(YMax - YMin);
		const float HalfLengthY = LengthY / 2;

		const float ZMin = FirstBlock.Center.Z - HalfBlockSize;
		const float ZMax = LastBlock.Center.Z + HalfBlockSize;
		const float LengthZ = FMath::Abs(ZMax - ZMin);
		const float HalfLengthZ = LengthZ / 2;

		FVector Center(XMax - HalfLengthX, YMax - HalfLengthY, ZMax - HalfLengthZ);
		Center -= GetActorLocation();

		URealtimeMeshBlueprintFunctionLibrary::AppendBoxMesh(FVector(HalfLengthX, HalfLengthY, HalfLengthZ), FTransform(Center), MeshData);
	}
	FORCEINLINE FBlockDataForGreedyMeshGeneration _GetBlockDataForGreedyMeshing(const FIntVector& Location)
	{
		const int32 Index = UGridUtilities::ConvertToArrayIndex(Location, m_WidthInBlocks);
		FBlockDataForGreedyMeshGeneration BlockData;
		const FVector BlockLocationLocal = ((FVector(Location) * (m_BlockSize)) - (m_OverlapCollision->GetUnscaledBoxExtent()) + m_BlockSize / 2);

		UGridUtilities::UnpackInt32ToInt16(m_Blocks[Index], BlockData.Block.Source, BlockData.Block.ID);

		BlockData.Index = Index;
		BlockData.RelativeLocation = Location;
		BlockData.Center = UGridUtilities::ConvertToWorldSpaceFVector(BlockLocationLocal, GetTransform());

		return BlockData;
	}
#pragma endregion

#pragma region Marching Cubes Algorithm
	void MarchBlocks(FRealtimeMeshSimpleMeshData& MeshData);
	FORCEINLINE void _GetBlockDataForMarchingCubes(const int& x, const int& y, const int& z, const FVector& CurrentBlockPositionInUnits, FBlockDataForMarchingCubes& CurrentVoxel) const;
#pragma endregion

private:

	UPROPERTY(BlueprintGetter = GetChunkPosInGrid, DisplayName = "Chunk Position In Grid", Category = "Stats")
	FIntVector m_ChunkPosInGrid;

	UPROPERTY(BlueprintGetter = GetChunkIndexInGrid, DisplayName = "Start X", Category = "Stats")
	int32 m_ChunkIndexInGrid;

	UPROPERTY(BlueprintGetter = GetStartX, DisplayName = "Start X", Category = "Stats")
	int32 m_StartX;

	UPROPERTY(BlueprintGetter = GetStartY, DisplayName = "Start Y", Category = "Stats")
	int32 m_StartY;

	UPROPERTY(BlueprintGetter = GetStartZ, DisplayName = "Start Z", Category = "Stats")
	int32 m_StartZ;

	UPROPERTY(BlueprintGetter = GetEndX, DisplayName = "End X", Category = "Stats")
	int32 m_EndX;

	UPROPERTY(BlueprintGetter = GetEndY, DisplayName = "End Y", Category = "Stats")
	int32 m_EndY;

	UPROPERTY(BlueprintGetter = GetEndZ, DisplayName = "End Z", Category = "Stats")
	int32 m_EndZ;

	UPROPERTY(BlueprintGetter = GetWidthInBlocks, DisplayName = "Width in Blocks", Category = "Stats")
	int32 m_WidthInBlocks;

	UPROPERTY(BlueprintGetter = GetHeightInBlocks, DisplayName = "Height in Blocks", Category = "Stats")
	int32 m_HeightInBlocks;

	// In units.
	UPROPERTY(BlueprintGetter = GetBlockSize, DisplayName = "Block Size", Category = "Stats")
	int32 m_BlockSize;

	TArray<int32> m_Blocks;

	UPROPERTY()
	ABuildingGrid* m_ContainingGrid;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* m_OverlapCollision;

	UPROPERTY(VisibleAnywhere)
	URealtimeMeshComponent* m_RealtimeMeshComponent;

	UPROPERTY()
	URealtimeMeshSimple* m_Mesh;

	UPROPERTY()
	UMaterialInstance* m_GridMaterial;
};