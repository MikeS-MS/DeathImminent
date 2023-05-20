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

UCLASS(Blueprintable)
class DEATHIMMINENT_API ABuildingGrid : public AActor
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Stats")
	bool m_GreedyMeshing = true;

public:
	//~Begin AActor section
	ABuildingGrid();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End Interface methods section

#pragma region Operations
	UFUNCTION(CallInEditor, Category = "Stats")
	void UpdateChunks();

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void TryHitBlock(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Operations")
	void TryPlaceBlock(const FVector& Location);
#pragma endregion Public

#pragma region Blueprint Getters
	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetBlockSize() const
	{
		return m_BlockSize;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetChunkWidthInBlocks() const
	{
		return m_ChunkWidthInBlocks;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetChunkHeightInBlocks() const
	{
		return m_ChunkHeightInBlocks;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetWidthInBlocks() const
	{
		return m_WidthInBlocks;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetHeightInBlocks() const
	{
		return m_HeightInBlocks;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetChunkWidthInUnits() const
	{
		return m_ChunkWidthInUnits;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetChunkHeightInUnits() const
	{
		return m_ChunkHeightInUnits;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetWidthInUnits() const
	{
		return m_WidthInUnits;
	}

	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Stats")
	const int32& GetHeightInUnits() const
	{
		return m_HeightInUnits;
	}
#pragma endregion Public

#pragma region Utility Methods
	UFUNCTION(BlueprintCallable, Category = "Utilities|Data")
	AChunk* GetChunkAtArrayPosition(const FIntVector& ChunkPosition)
	{
		const bool IsChunkPosValid = ((ChunkPosition.X >= 0 && ChunkPosition.X < m_ChunksAmountXY) && (ChunkPosition.Y >= 0 && ChunkPosition.Y < m_ChunksAmountXY)) && (ChunkPosition.Z >= 0 && ChunkPosition.Z < m_ChunksAmountZ);
		if (!IsChunkPosValid)
			return nullptr;

		return m_Chunks[UGridUtilities::ConvertToArrayIndex(ChunkPosition, m_ChunksAmountXY)];
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	void IsLocationInsideGridInUnits(const FVector& Location, UPARAM(ref) bool& OutResult) const
	{
		OutResult = IsLocationInsideGridInUnits(Location);
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities", DisplayName = "Is Location Inside Grid In Blocks")
	void IsLocationInsideGridInBlocks(const FIntVector& Location, UPARAM(ref) bool& OutResult) const
	{
		OutResult = IsLocationInsideGridInBlocks(Location);
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities", DisplayName = "Is Location Inside Grid In Blocks")
	void IsLocationInsideGridInBlocksXYZ(const int32& x, const int32& y, const int32& z, UPARAM(ref) bool& OutResult) const
	{
		OutResult = IsLocationInsideGridInBlocks(x, y, z);
	}

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	FBlockID GetBlockAtWorldLocation(const FVector& Location);

	UFUNCTION(BlueprintCallable, DisplayName = "Get Block At Grid Location", Category = "Utilities")
	FBlockID GetBlockAtGridLocation(const FIntVector& Location);

	UFUNCTION(BlueprintCallable, DisplayName = "Get Block At Grid Location", Category = "Utilities")
	FBlockID GetBlockAtGridLocationXYZ(const int& x, const int& y, const int& z);

	FORCEINLINE bool IsLocationInsideGridInUnits(const FVector& Location) const
	{
		const FVector& ActorLocation = GetTransform().GetLocation();
		const FVector& Extent = m_GridBounds->GetUnscaledBoxExtent();
		const FBox Box(ActorLocation - Extent, ActorLocation + Extent);
		return Box.IsInside(Location);
	}
	FORCEINLINE bool IsLocationInsideGridInBlocks(const FIntVector& Location) const
	{
		return IsLocationInsideGridInBlocks(Location.X, Location.Y, Location.Z);
	}
	FORCEINLINE bool IsLocationInsideGridInBlocks(const int& x, const int& y, const int& z) const
	{
		return ((x > -1 && x < m_WidthInBlocks) && (y > -1 && y < m_WidthInBlocks)) && (z > -1 && z < m_HeightInBlocks);
	}
#pragma endregion Public

#pragma region Operation Methods
	UFUNCTION(BlueprintCallable, Category = "Operations")
	void FillBlocks(int32 FromX = 0,  int32 Width = -1,  int32 FromY = 0,  int32 Depth = -1,  int32 FromZ = 0,  int32 Height = -1, const FBlockID& BlockID = FBlockID());
#pragma endregion Public

protected:

	//~Begin AActor section
	virtual void BeginPlay() override;
	//~End AActor section

#pragma region Blueprint Getters
	UFUNCTION(BlueprintGetter, meta = (BlueprintProtected), Category = "Data")
	TArray<AChunk*>& GetChunks()
	{
		return m_Chunks;
	}
#pragma endregion Protected

#pragma region Utility Methods
	UFUNCTION(BlueprintCallable, Category = "Utility", DisplayName = "Get Array Index From Location")
	void GetArrayIndexFromLocation(const FVector& Location, UPARAM(ref) int32& OutResult) const
	{
		OutResult = _GetArrayIndexFromLocation(Location);
	}

	FORCEINLINE int32 _GetArrayIndexFromLocation(const FVector& Location) const
	{
		const FVector& GridArrayUsableVector = UGridUtilities::ConvertToArrayUsableVector(Location, m_GridBounds->GetUnscaledBoxExtent());
		const FIntVector& ArrayIndexVector = UGridUtilities::ConvertToArrayIndexVector(GridArrayUsableVector, m_ChunkWidthInUnits, m_ChunkHeightInUnits);
		return UGridUtilities::ConvertToArrayIndex(ArrayIndexVector, m_ChunksAmountXY);
	}
#pragma endregion Protected

private:

	void SetSizes(const int32& BlockSize = BLOCK_SIZE_DEFAULT, const int32& ChunkWidthInBlocks = CHUNK_WIDTH_IN_BLOCKS_DEFAULT, const int32& ChunkHeightInBlocks = CHUNK_HEIGHT_IN_BLOCKS_DEFAULT, const int32& WidthInBlocks = WIDTH_IN_BLOCKS_DEFAULT, const int32& HeightInBlocks = HEIGHT_IN_BLOCKS_DEFAULT);
	void RecalculateSize();
	void SpawnChunks();
	FORCEINLINE void ExecuteFunctionIfInside(const FVector& Location, TFunctionRef<void(const int32&, const FVector&)> ExecutionFunction) const
	{
		if (!IsLocationInsideGridInUnits(Location))
			return;

		const int32 Index = _GetArrayIndexFromLocation(Location);
		if (!m_Chunks.IsValidIndex(Index))
			return;

		ExecutionFunction(Index, Location);
	}

protected:

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* m_GridBounds;

private:

	UPROPERTY(BlueprintGetter = GetBlockSize, EditAnywhere, DisplayName = "BlockSize", Category = "Stats", Replicated)
	int32 m_BlockSize;

	UPROPERTY(BlueprintGetter = GetChunkWidthInBlocks, EditAnywhere, DisplayName = "Chunk Width In Blocks", Category = "Stats", Replicated)
	int32 m_ChunkWidthInBlocks;

	UPROPERTY(BlueprintGetter = GetChunkHeightInBlocks, EditAnywhere, DisplayName = "Chunk Height In Blocks", Category = "Stats", Replicated)
	int32 m_ChunkHeightInBlocks;

	UPROPERTY(BlueprintGetter = GetWidthInBlocks, EditAnywhere, DisplayName = "Width In Blocks", Category = "Stats", Replicated)
	int32 m_WidthInBlocks;

	UPROPERTY(BlueprintGetter = GetHeightInBlocks, EditAnywhere, DisplayName = "Height In Blocks", Category = "Stats", Replicated)
	int32 m_HeightInBlocks;

	UPROPERTY(BlueprintGetter = GetChunkWidthInUnits, VisibleAnywhere, DisplayName = "Chunk Width In Units", Category = "Stats", Replicated)
	int32 m_ChunkWidthInUnits;

	UPROPERTY(BlueprintGetter = GetChunkHeightInUnits, VisibleAnywhere, DisplayName = "Chunk Height In Units", Category = "Stats", Replicated)
	int32 m_ChunkHeightInUnits;

	UPROPERTY(BlueprintGetter = GetWidthInUnits, VisibleAnywhere, DisplayName = "Width In Units", Category = "Stats", Replicated)
	int32 m_WidthInUnits;

	UPROPERTY(BlueprintGetter = GetHeightInUnits, VisibleAnywhere, DisplayName = "Height In Units", Category = "Stats", Replicated)
	int32 m_HeightInUnits;

	UPROPERTY(VisibleAnywhere, DisplayName = "Chunks Amount On XY", Category = "Stats", Replicated)
	int32 m_ChunksAmountXY;

	UPROPERTY(VisibleAnywhere, DisplayName = "Chunks Amount On Z", Category = "Stats", Replicated)
	int32 m_ChunksAmountZ;

	UPROPERTY(BlueprintGetter = GetChunks, DisplayName = "Chunks", Category = "Data")
	TArray<AChunk*> m_Chunks;
};