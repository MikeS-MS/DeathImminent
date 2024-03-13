#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseItem.h"
#include "ItemStructs.h"
#include "ItemInventoryComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class UItemInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UItemInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Category = "Operations", BlueprintCallable, Server, Reliable)
	void DropItemOnTopOfItem(const FItemSnapshot DroppedItem, const FItemSnapshot DroppedOnItem);
	void DropItemOnTopOfItem_Implementation(const FItemSnapshot DroppedItem, const FItemSnapshot DroppedOnItem);

	UFUNCTION(Category = "Operations", BlueprintCallable, Server, Reliable)
	void MoveItemToInventory(const FItemSnapshot ItemToBeMoved, const int32 AmountToMove, UItemInventoryComponent* InventoryToMoveInto);
	void MoveItemToInventory_Implementation(const FItemSnapshot ItemToBeMoved, const int32 AmountToMove, UItemInventoryComponent* InventoryToMoveInto);

	UFUNCTION(Category = "Operations", BlueprintCallable)
	FItemSnapshot CreateSnapshotForItemAt(const int32 Index);

	UFUNCTION(Category = "Operations", BlueprintCallable)
	bool IsItemAtIndexEqual(const FItemSnapshot& ItemSnapshot) const;

	UFUNCTION(Category = "Operations", BlueprintCallable)
	bool IsItemAtIndexEqualToID(const FBaseID& ItemID, const int32 Index) const;

	UFUNCTION(Category = "Utilities", BlueprintGetter)
	bool GetIsPlayerInventory() const
	{
		return mb__IsPlayerInventory;
	}

	UFUNCTION(Category = "Utilities", BlueprintGetter)
	bool GetAllowItemUsageFromDifferentInventory() const
	{
		return mb__AllowItemUsageFromDifferentInventory;
	}

	UFUNCTION(Category = "Utilities", BlueprintGetter)
	bool IsTargetInventory(const UItemInventoryComponent* InventoryToCheck) const
	{
		return m__TargetItemInventory == InventoryToCheck;
	}

	UFUNCTION(Category = "Utilities", BlueprintGetter)
	int32 GetSize() const
	{
		return m__Size;
	}

private:

	void __Setup();

	UFUNCTION(Server, Reliable)
	void __SetTargetInventory(UItemInventoryComponent* ItemInventoryComponent);
	void __SetTargetInventory_Implementation(UItemInventoryComponent* ItemInventoryComponent);

	UFUNCTION(Client, Reliable)
	void __ReceiveItemsForInventory(UItemInventoryComponent* ItemInventoryComponent, const TArray<ABaseItem*>& Items);
	void __ReceiveItemsForInventory_Implementation(UItemInventoryComponent* ItemInventoryComponent, const TArray<ABaseItem*>& Items);

	ABaseItem* __SetItem(ABaseItem* Item, const int32 Index);
	FAddItemOperationResult __AddItem(const FBaseID& ItemID, const int32 Amount, const int32 SetAtIndex = -1);

	int32 __GetFirstValidIndex(const FBaseID& ItemID);

private:

	/**
	* @brief If this is false the items array will be replicated to every other player, meaning this is a container everyone can access.
	* For Player inventories this needs to be true.
	*/
	UPROPERTY(DisplayName = "IsPlayerInventory", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetIsPlayerInventory, Replicated)
	bool mb__IsPlayerInventory = false;

	/**
	* @brief If this is true the items from this inventory can be used by other people while still in this inventory even if it's not theirs.
	* For Player inventories this needs to be false.
	*/
	UPROPERTY(DisplayName = "AllowItemUsageFromDifferentInventory", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetAllowItemUsageFromDifferentInventory, Replicated)
	bool mb__AllowItemUsageFromDifferentInventory = true;

	UPROPERTY(DisplayName = "Size", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetSize, Replicated, meta = (ClampMin = "0"))
	int32 m__Size = 40;

	UPROPERTY(Replicated)
	UItemInventoryComponent* m__TargetItemInventory;

	UPROPERTY(Replicated)
	TArray<ABaseItem*> m__Items;
};