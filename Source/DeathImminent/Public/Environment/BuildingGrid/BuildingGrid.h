// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Environment/BuildingGrid/Chunk.h"
#include "BuildingGrid.generated.h"

#define BLOCK_SIZE_DEFAULT 25
#define CHUNK_WIDTH_IN_BLOCKS_DEFAULT 30
#define CHUNK_HEIGHT_IN_BLOCKS_DEFAULT 30
#define WIDTH_IN_BLOCKS_DEFAULT 4320
#define HEIGHT_IN_BLOCKS_DEFAULT 300

class AChunk;

UENUM(BlueprintType)
enum class EMeshingAlgorithm : uint8
{
	MA_GreedyMeshing UMETA(DisplayName = "Greedy Meshing"),
	MA_MarchingCubes UMETA(DisplayName = "Marching Cubes"),
	MA_SurfaceNets   UMETA(DisplayName = "Surface Nets")
};

UCLASS(Blueprintable)
class DEATHIMMINENT_API ABuildingGrid : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Stats")
	EMeshingAlgorithm MeshingAlgorithm = EMeshingAlgorithm::MA_GreedyMeshing;

	UPROPERTY(EditAnywhere, Category = "Stats")
	bool Caves = false;

	UPROPERTY(EditAnywhere, Category = "Stats")
	int32 Seed = 1;

	UPROPERTY(EditAnywhere, Category = "Stats")
	int32 Octaves = 1;

	UPROPERTY(EditAnywhere, Category = "Stats")
	int32 Ceiling = 60;

	UPROPERTY(EditAnywhere, Category = "Stats")
	int32 Floor = 0;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Zoom = .5f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Lucanarity = 2.3f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Persistence = .6f;

public:

	//~Begin AActor section
	ABuildingGrid();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End Interface methods section

#pragma region Operations
	UFUNCTION(CallInEditor, Category = "Stats")
	void UpdateAllChunks();

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void TryHitBlock(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void TryPlaceBlock(const FVector& Location, const bool UpdateMesh = true);

	UFUNCTION(BlueprintCallable, Category = "Operations")
	FORCEINLINE bool SetBlockAtGridLocation(const FBlockID& BlockID, const int& x, const int& y, const int& z, const bool ShouldUpdateMesh = true)
	{
		AChunk* Chunk = nullptr;
		return SetBlockAtGridLocation_GetChunkRef(BlockID, x, y, z, Chunk, ShouldUpdateMesh);
	}

	UFUNCTION(BlueprintCallable, Category = "Operations")
	FORCEINLINE bool SetBlockAtGridLocation_GetChunkRef(const FBlockID& BlockID, const int& x, const int& y, const int& z, AChunk*& ModifiedChunk, const bool ShouldUpdateMesh = true)
	{
		const FIntVector ArrayIndex = UGridUtilities::ConvertToArrayIndexVector(FVector(x, y, z), m__ChunkWidthInBlocks, m__ChunkHeightInBlocks);
		const int32 ChunkIndex = UGridUtilities::ConvertToArrayIndex(ArrayIndex.X, ArrayIndex.Y, ArrayIndex.Z, m__ChunksAmountXY);

		if (!m__Chunks.IsValidIndex(ChunkIndex))
			return false;

		ModifiedChunk = m__Chunks[ChunkIndex];

		return ModifiedChunk->SetBlock(BlockID, x, y, z, true, ShouldUpdateMesh);
	}

	/**
	 * @brief Fills the grid with the given block in a Box formation made up from the given parameters.
	 */
	UFUNCTION(BlueprintCallable, Category = "Operations")
	void FillBlocks(int32 FromX = 0, int32 Width = -1, int32 FromY = 0, int32 Depth = -1, int32 FromZ = 0, int32 Height = -1, const FBlockID& BlockID = FBlockID());
	#pragma endregion Public

#pragma region Blueprint Getters
	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetBlockSize() const
	{
		return m__BlockSize;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetChunkWidthInBlocks() const
	{
		return m__ChunkWidthInBlocks;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetChunkHeightInBlocks() const
	{
		return m__ChunkHeightInBlocks;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetWidthInBlocks() const
	{
		return m__WidthInBlocks;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetHeightInBlocks() const
	{
		return m__HeightInBlocks;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetChunkWidthInUnits() const
	{
		return m__ChunkWidthInUnits;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetChunkHeightInUnits() const
	{
		return m__ChunkHeightInUnits;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetWidthInUnits() const
	{
		return m__WidthInUnits;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetHeightInUnits() const
	{
		return m__HeightInUnits;
	}
#pragma endregion Public

#pragma region Utility Methods
	/**
	 * Make sure to provide X, Y and Z in Grid Coordinates (x,y,z >= 0 && x,y,z <= Grid Width/Depth/Height) and not local chunk positions.
	 * Providing anything but Grid Coordinates will return unexpected results.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE FBlockLocations ConvertBlockGridPosToWorldPos(const int& x, const int& y, const int& z)
	{
		return UGridUtilities::ConvertBlockGridPosToWorldPos(x, y, z, m__BlockSize, m_GridBounds->GetUnscaledBoxExtent());
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities|Data", DisplayName = "Get Chunk At Array Position")
	FORCEINLINE AChunk* GetChunkAtArrayPosition(const FIntVector& ChunkPosition)
	{
		return GetChunkAtArrayPositionXYZ(ChunkPosition.X, ChunkPosition.Y, ChunkPosition.Z);
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities|Data", DisplayName = "Get Chunk At Array Position")
	FORCEINLINE AChunk* GetChunkAtArrayPositionXYZ(const int& x, const int& y, const int& z)
	{
		const bool IsChunkPosValid = ((x >= 0 && x < m__ChunksAmountXY) && (y >= 0 && y < m__ChunksAmountXY)) && (z >= 0 && z< m__ChunksAmountZ);
		if (!IsChunkPosValid)
			return nullptr;

		return m__Chunks[UGridUtilities::ConvertToArrayIndex(x, y, z, m__ChunksAmountXY)];
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE FBlockID GetBlockAtWorldLocation(const FVector& Location);

	UFUNCTION(BlueprintCallable, DisplayName = "Get Block At Grid Location", Category = "Utilities")
	FORCEINLINE FBlockID GetBlockAtGridLocation(const int& x, const int& y, const int& z);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FORCEINLINE bool IsLocationInsideGridInUnits(const FVector& Location) const
	{
		const FVector& ActorLocation = GetTransform().GetLocation();
		const FVector& Extent = m_GridBounds->GetUnscaledBoxExtent();
		const FBox Box(ActorLocation - Extent, ActorLocation + Extent);
		return Box.IsInside(Location);
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities", DisplayName = "Is Location Inside Grid In Blocks")
	FORCEINLINE bool IsLocationInsideGridInBlocks(const int& x, const int& y, const int& z) const
	{
		return ((x > -1 && x < m__WidthInBlocks) && (y > -1 && y < m__WidthInBlocks)) && (z > -1 && z < m__HeightInBlocks);
	}
#pragma endregion Public

protected:

	//~Begin AActor section
	virtual void BeginPlay() override;
	//~End AActor section

#pragma region Blueprint Getters
	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Data")
	TArray<AChunk*>& GetChunks()
	{
		return m__Chunks;
	}
#pragma endregion Protected

#pragma region Utility Methods
	UFUNCTION(BlueprintCallable, Category = "Utility", DisplayName = "Get Array Index From Location", meta = (BlueprintProtected))
	FORCEINLINE int32 _GetArrayIndexFromWorldLocation(const FVector& Location) const
	{
		const FVector& GridArrayUsableVector = UGridUtilities::ConvertToArrayUsableVector(Location, m_GridBounds->GetUnscaledBoxExtent());
		const FIntVector& ArrayIndexVector = UGridUtilities::ConvertToArrayIndexVector(GridArrayUsableVector, m__ChunkWidthInUnits, m__ChunkHeightInUnits);
		return UGridUtilities::ConvertToArrayIndex(ArrayIndexVector.X, ArrayIndexVector.Y, ArrayIndexVector.Z, m__ChunksAmountXY);
	}

	UFUNCTION(BlueprintCallable, Category = "Utility", DisplayName = "Get Array Index From Location", meta = (BlueprintProtected))
	FORCEINLINE int32 _GetArrayIndexFromGridLocation(const int& x, const int& y, const int& z) const
	{
		const FIntVector ContainingChunkIndexVector = UGridUtilities::ConvertToArrayIndexVector(FVector(x, y, z), m__ChunkWidthInBlocks, m__ChunkHeightInBlocks);
		return UGridUtilities::ConvertToArrayIndex(ContainingChunkIndexVector.X, ContainingChunkIndexVector.Y, ContainingChunkIndexVector.Z, m__ChunksAmountXY);
	}
#pragma endregion Protected

private:

#pragma region Test
	void __FillTestFromNoise();
	void __FillCaves();
#pragma endregion

	void __SetSizes(const int32& BlockSize = BLOCK_SIZE_DEFAULT, const int32& ChunkWidthInBlocks = CHUNK_WIDTH_IN_BLOCKS_DEFAULT, const int32& ChunkHeightInBlocks = CHUNK_HEIGHT_IN_BLOCKS_DEFAULT, const int32& WidthInBlocks = WIDTH_IN_BLOCKS_DEFAULT, const int32& HeightInBlocks = HEIGHT_IN_BLOCKS_DEFAULT);
	void __RecalculateSize();
	void __SpawnChunks();
	FORCEINLINE void __ExecuteFunctionIfInside(const FVector& Location, TFunctionRef<void(const int32&, const FVector&)> ExecutionFunction) const
	{
		if (!IsLocationInsideGridInUnits(Location))
			return;

		const int32 Index = _GetArrayIndexFromWorldLocation(Location);
		if (!m__Chunks.IsValidIndex(Index))
			return;

		ExecutionFunction(Index, Location);
	}

protected:

	UPROPERTY(VisibleAnywhere, DisplayName = "Grid Bounds Box Collision")
	UBoxComponent* m_GridBounds;

private:

	UPROPERTY(BlueprintGetter = GetBlockSize, EditAnywhere, DisplayName = "BlockSize", Category = "Stats", Replicated)
	int32 m__BlockSize;

	UPROPERTY(BlueprintGetter = GetChunkWidthInBlocks, EditAnywhere, DisplayName = "Chunk Width In Blocks", Category = "Stats", Replicated)
	int32 m__ChunkWidthInBlocks;

	UPROPERTY(BlueprintGetter = GetChunkHeightInBlocks, EditAnywhere, DisplayName = "Chunk Height In Blocks", Category = "Stats", Replicated)
	int32 m__ChunkHeightInBlocks;

	UPROPERTY(BlueprintGetter = GetWidthInBlocks, EditAnywhere, DisplayName = "Width In Blocks", Category = "Stats", Replicated)
	int32 m__WidthInBlocks;

	UPROPERTY(BlueprintGetter = GetHeightInBlocks, EditAnywhere, DisplayName = "Height In Blocks", Category = "Stats", Replicated)
	int32 m__HeightInBlocks;

	UPROPERTY(BlueprintGetter = GetChunkWidthInUnits, VisibleAnywhere, DisplayName = "Chunk Width In Units", Category = "Stats", Replicated)
	int32 m__ChunkWidthInUnits;

	UPROPERTY(BlueprintGetter = GetChunkHeightInUnits, VisibleAnywhere, DisplayName = "Chunk Height In Units", Category = "Stats", Replicated)
	int32 m__ChunkHeightInUnits;

	UPROPERTY(BlueprintGetter = GetWidthInUnits, VisibleAnywhere, DisplayName = "Width In Units", Category = "Stats", Replicated)
	int32 m__WidthInUnits;

	UPROPERTY(BlueprintGetter = GetHeightInUnits, VisibleAnywhere, DisplayName = "Height In Units", Category = "Stats", Replicated)
	int32 m__HeightInUnits;

	UPROPERTY(VisibleAnywhere, DisplayName = "Chunks Amount On XY", Category = "Stats", Replicated)
	int32 m__ChunksAmountXY;

	UPROPERTY(VisibleAnywhere, DisplayName = "Chunks Amount On Z", Category = "Stats", Replicated)
	int32 m__ChunksAmountZ;

	UPROPERTY(BlueprintGetter = GetChunks, DisplayName = "Chunks", Category = "Data")
	TArray<AChunk*> m__Chunks;
};