// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkUpdateQueueComponent.h"
#include "GridData.h"
#include "Environment/BuildingGrid/Chunk.h"
#include "Utilities/GameUtilities.h"
#include "GridStructs.h"
#include "BuildingGrid.generated.h"

class AChunk;

UENUM(BlueprintType)
enum class EMeshingAlgorithm : uint8
{
	MA_GreedyMeshing UMETA(DisplayName = "Greedy Meshing"),
	MA_MarchingCubes UMETA(DisplayName = "Marching Cubes"),
	MA_SurfaceNets   UMETA(DisplayName = "Surface Nets"),
	MA_CustomMeshing UMETA(DisplayName = "Custom Meshing")
};

UENUM(BlueprintType)
enum class EFillTest : uint8
{
	FT_Plane   UMETA(DisplayName = "Plane"),
	FT_Caves   UMETA(DisplayName = "Caves"),
	FT_Terrain UMETA(DisplayName = "Terrain")
};

DECLARE_LOG_CATEGORY_CLASS(LogBuildingGrid, Display, Display)

UCLASS(Blueprintable)
class DEATHIMMINENT_API ABuildingGrid : public AActor
{
	GENERATED_BODY()

	friend class AChunk;
	friend class UGridInfoComponent;
	friend class UGridModifyingComponent;

public:

	UPROPERTY(Category = "Stats", BlueprintReadWrite, EditAnywhere)
	EMeshingAlgorithm MeshingAlgorithm = EMeshingAlgorithm::MA_GreedyMeshing;

	UPROPERTY(Category = "Stats", EditAnywhere)
	int32 SurfaceNetsIterations = 10;

	UPROPERTY(Category = "Stats", EditAnywhere)
	int32 LookAheadBlocks = 3;

	UPROPERTY(Category = "Stats", EditAnywhere)
	EFillTest FillTestMethod = EFillTest::FT_Terrain;

	UPROPERTY(Category = "Stats", EditAnywhere)
	bool NoInterpolation = false;

	UPROPERTY(Category = "Stats", EditAnywhere)
	int32 Seed = 1;

	UPROPERTY(Category = "Stats", EditAnywhere)
	int32 Octaves = 1;

	UPROPERTY(Category = "Stats", EditAnywhere)
	int32 Ceiling = 60;

	UPROPERTY(Category = "Stats", EditAnywhere)
	int32 Floor = 0;

	UPROPERTY(Category = "Stats", EditAnywhere)
	float Zoom = .5f;

	UPROPERTY(Category = "Stats", EditAnywhere)
	float Lucanarity = 2.3f;

	UPROPERTY(Category = "Stats", EditAnywhere)
	float Persistence = .6f;
	
public:

	//~Begin AActor Interface section
	ABuildingGrid();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End AActor Interface methods section

	UFUNCTION(Category = "Operations", CallInEditor, BlueprintCallable)
	void UpdateAllChunks();

	/**
	 * @brief Fills the grid with the given block in a Box formation made up from the given parameters.
	 */
	UFUNCTION(Category = "Operations", BlueprintCallable)
	void FillBlocks(int32 FromX = 0, int32 Width = -1, int32 FromY = 0, int32 Depth = -1, int32 FromZ = 0, int32 Height = -1, const FBaseID& BlockID = FBaseID());

	/**
	 * Make sure to provide X, Y and Z in Grid Coordinates (x,y,z >= 0 && x,y,z <= Grid Width/Depth/Height) and not local chunk positions.
	 * Providing anything but Grid Coordinates will return unexpected results.
	 */
	UFUNCTION(Category = "Utilities", BlueprintCallable)
	FORCEINLINE FBlockLocations ConvertGridPosToWorldPos(const FIntVector& GridLocation) const
	{
		return UGridUtilities::ConvertBlockGridPosToWorldPos(GridLocation, m_GridData.BlockSize, m_GridBounds->GetUnscaledBoxExtent());
	}

	UFUNCTION(Category = "Utilities", BlueprintCallable)
	FORCEINLINE FIntVector ConvertWorldPosToGridPos(const FVector& Location) const
	{
		return FIntVector((Location + (m_GridData.SizeInUnits / 2)) / m_GridData.BlockSize);
	}

	UFUNCTION(Category = "Utilities", BlueprintCallable)
	FORCEINLINE bool IsLocationInsideWorldLocation(const FVector& Location) const
	{
		const FVector& ActorLocation = GetTransform().GetLocation();
		const FVector& Extent = m_GridBounds->GetUnscaledBoxExtent();
		const FBox Box(ActorLocation - Extent, ActorLocation + Extent);
		return Box.IsInside(Location);
	}

	UFUNCTION(Category = "Utilities", BlueprintCallable)
	FORCEINLINE bool IsLocationInsideGridLocation(const FIntVector& GridLocation) const
	{
		const FIntVector SizeInBlocks = m_GridData.SizeInBlocks;
		return ((GridLocation.X > -1 && GridLocation.X < SizeInBlocks.X) && (GridLocation.Y > -1 && GridLocation.Y < SizeInBlocks.Y)) && (GridLocation.Z > -1 && GridLocation.Z < SizeInBlocks.Z);
	}

	UFUNCTION(Category = "Utilities", BlueprintCallable)
	FORCEINLINE bool IsLocationInsideWorldLocationAndChunkExists(const FVector& Location, int32& Index) const
	{
		return IsLocationInsideGridLocationAndChunkExists(ConvertWorldPosToGridPos(Location), Index);
	}

	UFUNCTION(Category = "Utilities", BlueprintCallable)
	FORCEINLINE bool IsLocationInsideGridLocationAndChunkExists(const FIntVector& Location, int32& Index) const
	{
		if (!IsLocationInsideGridLocation(Location))
			return false;

		const int32 TempIndex = _GetArrayIndexFromGridLocation(Location);
		if (!m__Chunks.IsValidIndex(TempIndex))
			return false;

		Index = TempIndex;
		return true;
	}

	FGridData GetGridData()
	{
		return m_GridData;
	}

	static ABuildingGrid* GetInstance();

protected:

	UPROPERTY(DisplayName = "GridData", Category = "Stats", BlueprintReadOnly, EditAnywhere, Replicated)
	FGridData m_GridData;

	UPROPERTY(DisplayName = "GridBoundsBoxCollision", Category = "Data", BlueprintReadOnly, VisibleAnywhere)
	UBoxComponent* m_GridBounds;

	UPROPERTY()
	UChunkUpdateQueueComponent* m_ChunkUpdateQueueComponent;

protected:

	//~Begin AActor section
	virtual void BeginPlay() override;
	//~End AActor section

	UFUNCTION(DisplayName = "GetArrayIndexFromWorldLocation", Category = "Utilities", BlueprintCallable, meta = (BlueprintProtected))
	FORCEINLINE int32 _GetArrayIndexFromWorldLocation(const FVector& Location) const
	{
		return _GetArrayIndexFromGridLocation(ConvertWorldPosToGridPos(Location));
	}

	UFUNCTION(DisplayName = "GetArrayIndexFromGridLocation", Category = "Utilities", BlueprintCallable, meta = (BlueprintProtected))
	FORCEINLINE int32 _GetArrayIndexFromGridLocation(const FIntVector& GridLocation) const
	{
		const FIntVector ChunksAmount = m_GridData.ChunksAmount;
		const FIntVector ContainingChunkIndexVector = UGridUtilities::ConvertToArrayIndexVector(FVector(GridLocation), m_GridData.ChunkSizeInBlocks);
		return UGridUtilities::Convert3DPosTo1DArrayIndex(ContainingChunkIndexVector, ChunksAmount.X, ChunksAmount.Y);
	}

	UFUNCTION(DisplayName = "GetChunkAtGridLocation", Category = "Utilities", BlueprintCallable, meta = (BlueprintProtected))
	AChunk* _GetChunkAtGridLocation(const FIntVector& GridLocation);

	UFUNCTION(DisplayName = "GetChunkAtGridLocation", Category = "Utilities", BlueprintCallable, meta = (BlueprintProtected))
	AChunk* _GetChunkChunkArrayLocation(const FIntVector& ArrayLocation);

	UFUNCTION(DisplayName = "GetBlockAtGridLocation", Category = "Operations", BlueprintCallable, meta = (BlueprintProtected))
	FBlock _GetBlockAtGridLocation(const FIntVector& GridLocation, bool& Result);

	UFUNCTION(Category = "Data", BlueprintGetter, meta = (BlueprintProtected))
	TArray<AChunk*>& GetChunks()
	{
		return m__Chunks;
	}

private:

	void __SpawnChunks();

	UFUNCTION()
	void __RecalculateSize();

	/**
	 * @param Increments If set to true, @Fullness will be used to increment the Fullness of the block, if false it will set it to the @Fullness value
	 * @param Force If this is set to true it will set the desired block without checking if it's possible.
	 */
	FSetBlockOperationResult __SetBlockAtWorldLocation(const FVector& WorldLocation, 
													   const FBaseID& BlockID, 
													   const double Fullness = 0.0, 
													   const bool Increments = true,
													   const bool Force = false, 
													   const bool ShouldUpdate = true);

	/**
	 * @param Increments If set to true, @Fullness will be used to increment the Fullness of the block, if false it will set it to the @Fullness value
	 * @param Force If this is set to true it will set the desired block without checking if it's possible.
	 */
	FSetBlockOperationResult __SetBlockAtGridLocation(const FIntVector& GridLocation, 
													  const FBaseID& BlockID, 
													  const double Fullness = 0.0, 
													  const bool Increments = true, 
													  const bool Force = false, 
													  const bool ShouldUpdate = true);

	void __FillTestFromNoise();
	void __FillCaves();


private:
	UPROPERTY(BlueprintGetter = GetChunks, Replicated)
	TArray<AChunk*> m__Chunks;
};