// Copyright MikeSMediaStudios™

#include "Environment/WorldGrid/GridModifyingComponent.h"
#include "Environment/WorldGrid/WorldGrid.h"
#include "Environment/WorldGrid/GridStructs.h"
#include "Utilities/GameUtilities.h"

UGridModifyingComponent::UGridModifyingComponent()
{
	SetIsReplicatedByDefault(true);
}

void UGridModifyingComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UGridModifyingComponent::BreakFromGridTest(const FVector& Location)
{
	CHECK_INSTANCE(AWorldGrid, WorldGrid)

	const FSetBlockOperationResult SetBlockOperationResult = WorldGrid->__HitBlockAtWorldLocation(Location, 1);
}

void UGridModifyingComponent::AddToGridTest(const FVector& Location, const FBaseID& BlockId, const int32 Amount)
{
	_AddBlockToGrid(Location, BlockId , Amount);
}

void UGridModifyingComponent::AddToGridMultipleTest(const FVector& Location,
	const FMultipleBlockEditing& MultipleBlocks, const FBaseID& BlockId)
{
	CHECK_INSTANCE(AWorldGrid, BuildingGrid)

	const int32 AmountLeft = BuildingGrid->__SetBlockMultipleAtWorldLocation(Location, MultipleBlocks, BlockId);

}

void UGridModifyingComponent::_AddBlockToGrid_Implementation(const FVector& Location, const FBaseID& BlockID,
                                                             const int32 Amount)
{
	CHECK_INSTANCE(AWorldGrid, BuildingGrid)


	// TODO: Amount should be set as per block basis, so we should be looking it up from the block manager.
	const FSetBlockOperationResult SetBlockOperationResult = BuildingGrid->__SetBlockAtWorldLocation(Location, BlockID, Amount);

	//if (!SetBlockOperationResult.Completed)
	//	return;

	//m_OnBlockAddedToGrid.Broadcast(BlockID, Amount, SetBlockOperationResult.LeftOverFullness * 100);
}

void UGridModifyingComponent::_RemoveBlockFromGrid_Implementation(const FVector& Location, const int32 Amount)
{
	CHECK_INSTANCE(AWorldGrid, BuildingGrid)

	//const FSetBlockOperationResult SetBlockOperationResult = BuildingGrid->__SetBlockAtWorldLocation(Location, BlockID, Amount * 0.1);

}
