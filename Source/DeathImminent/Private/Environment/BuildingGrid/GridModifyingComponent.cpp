#include "Environment/BuildingGrid/GridModifyingComponent.h"
#include "Environment/BuildingGrid/GridUtilities.h"
#include "BuildingGridSubsystem.h"

UGridModifyingComponent::UGridModifyingComponent()
{
	SetIsReplicated(true);
}

void UGridModifyingComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UGridModifyingComponent::AddToGridTest(const FVector& Location, const int32 Amount)
{
	_AddBlockToGrid(Location, FBaseID(1), Amount);
}

void UGridModifyingComponent::_AddBlockToGrid_Implementation(const FVector& Location, const FBaseID& BlockID,
                                                             const int32 Amount)
{
	CHECK_INSTANCE(ABuildingGrid, BuildingGrid)


	// TODO: Amount should be set as per block basis, so we should be looking it up from the block manager.
	const FSetBlockOperationResult SetBlockOperationResult = BuildingGrid->__SetBlockAtWorldLocation(Location, BlockID, Amount * 0.01);

	if (!SetBlockOperationResult.Completed)
		return;

	m_OnBlockAddedToGrid.Broadcast(BlockID, Amount, SetBlockOperationResult.LeftOverFullness * 100);
}

void UGridModifyingComponent::_RemoveBlockFromGrid_Implementation(const FVector& Location, const int32 Amount)
{
	CHECK_INSTANCE(ABuildingGrid, BuildingGrid)

	//const FSetBlockOperationResult SetBlockOperationResult = BuildingGrid->__SetBlockAtWorldLocation(Location, BlockID, Amount * 0.1);

}
