// Copyright MikeSMediaStudios�

#include "Items/ItemInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Systems/ItemManager.h"
#include "NPC/SurvivalPlayer.h"
#include "Utilities/GameUtilities.h"

UItemInventoryComponent::UItemInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	__Setup();
}

void UItemInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemInventoryComponent, mb_IsPlayerInventory)
	DOREPLIFETIME(UItemInventoryComponent, mb_AllowItemUsageFromDifferentInventory)
	DOREPLIFETIME(UItemInventoryComponent, m_Size)
	DOREPLIFETIME_CONDITION(UItemInventoryComponent, m__TargetItemInventory, COND_OwnerOnly)
	DOREPLIFETIME_CONDITION(UItemInventoryComponent, m__Items, COND_OwnerOnly)
}

void UItemInventoryComponent::DropItemOnTopOfItem_Implementation(const FItemSnapshot DroppedItem, const FItemSnapshot DroppedOnItem)
{
	if (!IsValid(DroppedItem.ItemID) && !IsValid(DroppedItem.ItemID))
		return;

	UItemInventoryComponent* DroppedItemInventory = DroppedItem.OwningInventory;
	UItemInventoryComponent* DroppedOnItemInventory = DroppedOnItem.OwningInventory;

	if (!IsValid(DroppedItemInventory) || !IsValid(DroppedOnItemInventory))
		return;

	const int32 DroppedItemIndex = DroppedItem.Index;
	const int32 DroppedOnItemIndex = DroppedOnItem.Index;

	if (!DroppedItemInventory->IsItemAtIndexEqual(DroppedItem) || 
		!DroppedOnItemInventory->IsItemAtIndexEqual(DroppedOnItem))
		return;

	if (DroppedItemInventory == DroppedOnItemInventory)
	{
		DroppedOnItemInventory->m__Items.Swap(DroppedItemIndex, DroppedOnItemIndex);
		return;
	}

	const bool IsDroppedItemFromPlayerInventory = DroppedItemInventory->GetIsPlayerInventory();
	const UItemInventoryComponent* ThePlayerInventory = IsDroppedItemFromPlayerInventory ? DroppedItemInventory : DroppedOnItemInventory;
	const UItemInventoryComponent* TheOtherInventory = IsDroppedItemFromPlayerInventory ? DroppedOnItemInventory : DroppedItemInventory;

	// Can't be doing anything if none of the inventories are a player inventory as that means someone might be cheating.
	if (!ThePlayerInventory->GetIsPlayerInventory())
		return;

	// Well somebody is probably cheating or the game is out of sync as the target inventory needs to be set when trying to move items between inventories.
	if (!ThePlayerInventory->IsTargetInventory(TheOtherInventory))
		return;

	ABaseItem* DroppedItemInstance = DroppedItemInventory->m__Items[DroppedItemIndex];
	ABaseItem* DroppedOnItemInstance = DroppedOnItemInventory->m__Items[DroppedOnItemIndex];

	const bool DroppedItemValid = IsValid(DroppedItemInstance);
	const bool DroppedOnItemValid = IsValid(DroppedOnItemInstance);

	if (!DroppedItemValid && DroppedOnItemValid)
		return;

	if ((!DroppedItemValid || !DroppedOnItemValid) || (DroppedItemInstance->GetItemID() != DroppedOnItemInstance->GetItemID()))
	{
		DroppedItemInventory->m__Items[DroppedItemIndex] = DroppedOnItemInstance;
		DroppedOnItemInventory->m__Items[DroppedOnItemIndex] = DroppedItemInstance;
		return;
	}

	const FAddItemOperationResult Result = DroppedOnItemInstance->__SetCurrentAmount(DroppedItemInstance->m__CurrentAmount);

	if (!Result.Success)
		return;

	if (Result.AmountLeft > 0)
	{
		DroppedItemInstance->__SetCurrentAmount(Result.AmountLeft, false, true);
		return;
	}

	DroppedItemInstance->Destroy();
	DroppedItemInventory->m__Items[DroppedItemIndex] = nullptr;
}

void UItemInventoryComponent::MoveItemToInventory_Implementation(const FItemSnapshot ItemToBeMoved, const int32 AmountToMove,
	UItemInventoryComponent* InventoryToMoveInto)
{
	const FBaseID ItemID = ItemToBeMoved.ItemID;
	if (!IsValid(ItemID))
		return;

	UItemInventoryComponent* ItemToBeMovedInventory = ItemToBeMoved.OwningInventory;

	if (!IsValid(InventoryToMoveInto) || !IsValid(ItemToBeMovedInventory) || (ItemToBeMovedInventory == InventoryToMoveInto))
		return;

	if (!ItemToBeMovedInventory->IsItemAtIndexEqual(ItemToBeMoved))
		return;

	if (AmountToMove > ItemToBeMoved.Amount || AmountToMove == 0)
		return;

	const bool IsInventoryToMoveIntoPlayerInventory = InventoryToMoveInto->GetIsPlayerInventory();
	const UItemInventoryComponent* ThePlayerInventory = IsInventoryToMoveIntoPlayerInventory ? InventoryToMoveInto : ItemToBeMovedInventory;
	const UItemInventoryComponent* TheOtherInventory = IsInventoryToMoveIntoPlayerInventory ? ItemToBeMovedInventory : InventoryToMoveInto;


	// Can't be doing anything if none of the inventories are a player inventory as that means someone might be cheating.
	if (!ThePlayerInventory->GetIsPlayerInventory())
		return;

	// Well somebody is probably cheating or the game is out of sync as the target inventory needs to be set when trying to move items between inventories.
	if (!ThePlayerInventory->IsTargetInventory(TheOtherInventory))
		return;

	ABaseItem* ItemToBeMovedInstance = ItemToBeMovedInventory->m__Items[ItemToBeMoved.Index];

	if (!IsValid(ItemToBeMovedInstance))
		return;

	const int32 CurrentAmount = ItemToBeMovedInstance->GetCurrentAmount();
	FAddItemOperationResult Result;
	Result.AmountLeft = AmountToMove == -1 ? CurrentAmount : AmountToMove;
	const bool EqualsCurrentAmount = Result.AmountLeft == CurrentAmount;

	Result = InventoryToMoveInto->__AddItem(ItemID, Result.AmountLeft);

	if (EqualsCurrentAmount)
	{
		if (Result.AmountLeft == 0)
		{
			ItemToBeMovedInstance->Destroy();
			ItemToBeMovedInventory->m__Items[ItemToBeMoved.Index] = nullptr;
			return;
		}
	}

	ItemToBeMovedInstance->__SetCurrentAmount(Result.AmountLeft == 0 ? CurrentAmount - AmountToMove : Result.AmountLeft, false, true);
}

void UItemInventoryComponent::UseItemAtIndex_Implementation(ASurvivalPlayer* User, const FItemSnapshot ItemSnapshot)
{
	UItemInventoryComponent* InventoryToUseItemIn = ItemSnapshot.OwningInventory;
	if (!IsValid(User) || !IsValid(InventoryToUseItemIn))
		return;

	if (!InventoryToUseItemIn->m__Items.IsValidIndex(ItemSnapshot.Index))
		return;

	if (!InventoryToUseItemIn->IsItemAtIndexEqual(ItemSnapshot))
		return;

	ABaseItem* Item = InventoryToUseItemIn->m__Items[ItemSnapshot.Index];
	if (!IsValid(Item))
		return;

	UItemInventoryComponent* UserItemInventory = User->GetItemInventoryComponent();

	if (!IsValid(UserItemInventory))
		return;

	if (UserItemInventory != InventoryToUseItemIn)
	{
		if (!UserItemInventory->mb_IsPlayerInventory)
			return;

		if (!UserItemInventory->IsTargetInventory(InventoryToUseItemIn))
			return;

		if (!InventoryToUseItemIn->mb_AllowItemUsageFromDifferentInventory)
			return;
	}


	Item->__UseItem(User);
}

FItemSnapshot UItemInventoryComponent::CreateSnapshotForItemAt(const int32 Index)
{
	if (!m__Items.IsValidIndex(Index))
		return FItemSnapshot(this, FBaseID(), 0, Index);

	const ABaseItem* Item = m__Items[Index];

	if (!IsValid(Item))
		return FItemSnapshot(this, FBaseID(), 0, Index);

	return FItemSnapshot(this, Item->m__ItemID, Item->m__CurrentAmount, Index);
}

bool UItemInventoryComponent::IsItemAtIndexEqual(const FItemSnapshot& ItemSnapshot) const
{
	if (ItemSnapshot.OwningInventory != this)
		return false;

	if (!m__Items.IsValidIndex(ItemSnapshot.Index))
		return false;

	const ABaseItem* Item = m__Items[ItemSnapshot.Index];
	const bool IsItemValid = IsValid(Item);
	const bool IsItemIDEqual = (IsItemValid ? Item->GetItemID() : FBaseID::InvalidId) == ItemSnapshot.ItemID;
	const bool IsItemAmountEqual = (IsItemValid ? Item->GetCurrentAmount() : 0) == ItemSnapshot.Amount;

	return IsItemIDEqual && IsItemAmountEqual;
}

bool UItemInventoryComponent::IsItemAtIndexEqualToID(const FBaseID& ItemID, const int32 Index) const
{
	if (!m__Items.IsValidIndex(Index))
		return false;

	const ABaseItem* Item = m__Items[Index];
	return (IsValid(Item) ? Item->GetItemID() : FBaseID::InvalidId) == ItemID;
}

void UItemInventoryComponent::__Setup()
{
	m__Items.SetNum(m_Size);
}

void UItemInventoryComponent::__SetTargetInventory_Implementation(UItemInventoryComponent* ItemInventoryComponent)
{
	if (!mb_IsPlayerInventory)
		return;

	if (ItemInventoryComponent == this)
		return;

	if (IsValid(m__TargetItemInventory))
		m__TargetItemInventory->m__TargetedByInventories.Remove(this);

	if (ItemInventoryComponent->mb_IsPlayerInventory && !ItemInventoryComponent->mb_AllowOtherPlayersToOpen)
		return;

	m__TargetItemInventory = ItemInventoryComponent;

	if (!IsValid(m__TargetItemInventory))
		return;

	m__TargetItemInventory->m__TargetedByInventories.AddUnique(this);
	__ReceiveItemsForInventory(m__TargetItemInventory, m__TargetItemInventory->m__Items);
}

void UItemInventoryComponent::__ReceiveItemsForInventory_Implementation(UItemInventoryComponent* ItemInventoryComponent,
																		const TArray<ABaseItem*>& Items)
{
	if (!IsValid(ItemInventoryComponent))
		return;

	ItemInventoryComponent->m__Items = Items;
}

void UItemInventoryComponent::__SetItem_Implementation(ABaseItem* Item, const int32 Index)
{
	if (!m__Items.IsValidIndex(Index))
		return;

	ABaseItem* PreviousItem = m__Items[Index];

	if (IsValid(PreviousItem))
		PreviousItem->Destroy();

	m__Items[Index] = Item;

	if (!IsValid(Item))
		return;

	AActor* Owner = GetOwner();
	Item->SetOwner(Owner);
	Item->AttachToActor(Owner, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false));
	Item->m__OwningInventory = this;
	Item->m__ItemIndex = Index;
}

FAddItemOperationResult UItemInventoryComponent::__AddItem(const FBaseID& ItemID, const int32 Amount, const int32 SetAtIndex)
{
	FAddItemOperationResult Result;
	Result.AmountLeft = Amount;

	CHECK_INSTANCE_RETURN(UItemManager, ItemManager, Result)

	FBaseID CorrectedID = ItemID;
	if (!ItemManager->IsItemValid(ItemID, CorrectedID))
		return Result;

	int32 AmountLeft = Amount;
	int32 StartIndex = SetAtIndex;

	if (!m__Items.IsValidIndex(StartIndex))
		StartIndex = __GetFirstValidIndex(CorrectedID);

	while (AmountLeft > 0)
	{
		if (!m__Items.IsValidIndex(StartIndex))
			break;

		ABaseItem* Item = m__Items[StartIndex];

		if (IsValid(Item))
		{
			Result = Item->__SetCurrentAmount(AmountLeft);
			AmountLeft = Result.AmountLeft;
		}
		else
		{
			Item = UItemManager::GetInstance()->__SpawnItem(CorrectedID, GetWorld());

			__SetItem(Item, StartIndex);
			Result = Item->__SetCurrentAmount(AmountLeft);
			AmountLeft = Result.AmountLeft;
		}

		StartIndex = __GetFirstValidIndex(CorrectedID);
	}

	return Result;
}

int32 UItemInventoryComponent::__GetFirstValidIndex(const FBaseID& ItemID)
{
	int32 EmptyIndex = -1;

	ABaseItem* Item = nullptr;
	for (int i = 0; i < m__Items.Num(); i++)
	{
		Item = m__Items[i];

		if (IsValid(Item))
		{
			if (!Item->IsFull() && Item->GetItemID() == ItemID)
				return i;
		}
		else
		{
			if (EmptyIndex < 0)
				EmptyIndex = i;
		}
	}

	return EmptyIndex;
}
