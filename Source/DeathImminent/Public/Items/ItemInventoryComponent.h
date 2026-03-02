// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseItem.h"
#include "ItemStructs.h"
#include "ItemInventoryComponent.generated.h"

class ASurvivalPlayer;

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class DEATHIMMINENT_API UItemInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class FInventoryAddDropMoveItemTest;
	friend class ABaseItem;
	friend class UBaseItemMenu;

public:

	UItemInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Category = "Operations", BlueprintCallable, Server, Reliable)
	void DropItemOnTopOfItem(const FItemSnapshot DroppedItem, const FItemSnapshot DroppedOnItem);
	void DropItemOnTopOfItem_Implementation(const FItemSnapshot DroppedItem, const FItemSnapshot DroppedOnItem);

	/**
	 * @param AmountToMove If this is set to -1 it will move the entire stack, if an amount is supplied and it's more than the item's amount it will not move it
	 */
	UFUNCTION(Category = "Operations", BlueprintCallable, Server, Reliable)
	void MoveItemToInventory(const FItemSnapshot ItemToBeMoved, const int32 AmountToMove, UItemInventoryComponent* InventoryToMoveInto);
	void MoveItemToInventory_Implementation(const FItemSnapshot ItemToBeMoved, const int32 AmountToMove, UItemInventoryComponent* InventoryToMoveInto);

	UFUNCTION(Category = "Operations", BlueprintCallable, Server, Unreliable)
	void UseItemAtIndex(ASurvivalPlayer* User, const FItemSnapshot ItemSnapshot);
	void UseItemAtIndex_Implementation(ASurvivalPlayer* User, const FItemSnapshot ItemSnapshot);

	UFUNCTION(Category = "Operations", BlueprintCallable)
	FItemSnapshot CreateSnapshotForItemAt(const int32 Index);

	UFUNCTION(Category = "Operations", BlueprintCallable)
	bool IsItemAtIndexEqual(const FItemSnapshot& ItemSnapshot) const;

	UFUNCTION(Category = "Utilities", BlueprintCallable)
	bool IsItemAtIndexEqualToID(const FBaseID& ItemID, const int32 Index) const;

	UFUNCTION(Category = "Stats", BlueprintGetter)
	bool GetIsEquippableInventory() const
	{
		return mb_IsEquippableInventory;
	}

	UFUNCTION(Category = "Stats", BlueprintGetter)
	bool GetIsPlayerInventory() const
	{
		return mb_IsPlayerInventory;
	}

	UFUNCTION(Category = "Stats", BlueprintGetter)
	bool GetAllowOtherPlayersToOpen() const
	{
		return mb_AllowOtherPlayersToOpen;
	}

	UFUNCTION(Category = "Stats", BlueprintGetter)
	bool GetAllowItemUsageFromDifferentInventory() const
	{
		return mb_AllowItemUsageFromDifferentInventory;
	}

	UFUNCTION(Category = "Utilities", BlueprintGetter)
	bool IsTargetInventory(const UItemInventoryComponent* InventoryToCheck) const
	{
		return m__TargetItemInventory == InventoryToCheck;
	}

	UFUNCTION(Category = "Stats", BlueprintGetter)
	int32 GetSize() const
	{
		return m_Size;
	}

protected:

	/**
	 * @brief If this is true, this means that this is an inventory that's supposed to replicate its items to other players as they are supposed to be seen when inside.
	 */
	UPROPERTY(DisplayName = "IsEquippableInventory", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetIsEquippableInventory, Replicated)
	bool mb_IsEquippableInventory = false;

	/**
	* @brief If this is false the items array will be replicated to every other player, meaning this is a container everyone can access.
	* For Player inventories this needs to be true.
	*/
	UPROPERTY(DisplayName = "IsPlayerInventory", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetIsPlayerInventory, Replicated)
	bool mb_IsPlayerInventory = false;

	/**
	 * @brief If this is true, it allows other players to open this inventory. This is only considered if this inventory is a player inventory, otherwise this option is ignored.
	 */
	UPROPERTY(DisplayName = "AllowOtherPlayersToOpen", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetAllowOtherPlayersToOpen, Replicated)
	bool mb_AllowOtherPlayersToOpen = false;

	/**
	* @brief If this is true the items from this inventory can be used by other people while still in this inventory even if it's not theirs.
	* For Player inventories this needs to be false.
	*/
	UPROPERTY(DisplayName = "AllowItemUsageFromDifferentInventory", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetAllowItemUsageFromDifferentInventory, Replicated)
	bool mb_AllowItemUsageFromDifferentInventory = true;

	UPROPERTY(DisplayName = "Size", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetSize, Replicated, meta = (ClampMin = "0"))
	int32 m_Size = 40;

private:

	void __Setup();

	UFUNCTION(Server, Reliable)
	void __SetTargetInventory(UItemInventoryComponent* ItemInventoryComponent);
	void __SetTargetInventory_Implementation(UItemInventoryComponent* ItemInventoryComponent);

	UFUNCTION(Client, Reliable)
	void __ReceiveItemsForInventory(UItemInventoryComponent* ItemInventoryComponent, const TArray<ABaseItem*>& Items);
	void __ReceiveItemsForInventory_Implementation(UItemInventoryComponent* ItemInventoryComponent, const TArray<ABaseItem*>& Items);

	UFUNCTION(Server, Reliable)
	void __SetItem(ABaseItem* Item, const int32 Index);
	void __SetItem_Implementation(ABaseItem* Item, const int32 Index);

	FAddItemOperationResult __AddItem(const FBaseID& ItemID, const int32 Amount, const int32 SetAtIndex = -1);

	int32 __GetFirstValidIndex(const FBaseID& ItemID);

private:

	UPROPERTY(DisplayName = "TargetItemInventory", Category = "Data", VisibleAnywhere, Replicated)
	UItemInventoryComponent* m__TargetItemInventory;

	UPROPERTY(DisplayName = "TargetedByInventories", Category = "Data", VisibleAnywhere)
	TArray<UItemInventoryComponent*> m__TargetedByInventories;

	UPROPERTY(Replicated)
	TArray<ABaseItem*> m__Items;
};
