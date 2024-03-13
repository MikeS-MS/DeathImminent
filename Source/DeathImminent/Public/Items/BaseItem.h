// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utilities/GeneralStructs.h"
#include "ItemStructs.h"
#include "BaseItem.generated.h"

class UItemInventoryComponent;

UCLASS(BlueprintType, Blueprintable, Abstract)
class DEATHIMMINENT_API ABaseItem : public AActor
{
	GENERATED_BODY()

	friend class UItemInventoryComponent;
	friend class UItemManager;

public:

	ABaseItem();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief Override this to do custom checks to know if your item can be used.
	 */
	UFUNCTION(Category = "Utilities", BlueprintNativeEvent)
	bool CanBeUsed();
	virtual bool CanBeUsed_Implementation();

	UFUNCTION(BlueprintCallable)
	bool IsFull();

	UFUNCTION(BlueprintGetter)
	bool GetIsConsumable() const
	{
		return mb_IsConsumable;
	}

	UFUNCTION(BlueprintGetter)
	int32 GetCurrentAmount() const
	{
		return m__CurrentAmount;
	}

	UFUNCTION(BlueprintGetter)
	int32 GetMaxAmount() const
	{
		return m_MaxAmount;
	}

	UFUNCTION(BlueprintCallable)
	FBaseID GetItemID() const
	{
		return m__ItemID;
	}

	//UFUNCTION(BlueprintCallable, BlueprintGetter)
	//FItemInformation GetItemInfo()
	//{
	//	return m_ItemInfo;
	//}

	UFUNCTION(BlueprintGetter)
	UItemInventoryComponent* GetOwningInventory()
	{
		return m__OwningInventory;
	}

protected:

	UPROPERTY(DisplayName = "IsConsumable", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetIsConsumable, Replicated)
	bool mb_IsConsumable = true;

	UPROPERTY(DisplayName = "MaxAmount", Category = "Stats", EditDefaultsOnly, BlueprintGetter = GetMaxAmount, Replicated)
	int32 m_MaxAmount = 20;

protected:

	virtual void BeginPlay() override;

	/**
	 * @brief Override this to implement the item behaviour when it's used, this gets called the server only. \n
	 * Make sure to not call this manually on clients as it can lead to desync.
	 */
	UFUNCTION(DisplayName = "OnUseItem_Server", Category = "Event", BlueprintNativeEvent, meta = (BlueprintProtected))
	bool _OnUseItem_Server();
	virtual bool _OnUseItem_Server_Implementation();

	/**
	 * @brief Override this to implement visual behaviour on clients, this gets called on every client (that is in range)
	 */
	UFUNCTION(DisplayName = "OnUseItem_Client", Category = "Event", BlueprintNativeEvent, meta = (BlueprintProtected))
	void _OnUseItem_Client();
	virtual void _OnUseItem_Client_Implementation();

private:

	UFUNCTION(DisplayName = "UseItem", Category = "Operations", Server, Reliable)
	void __UseItem(/* const ABaseEntity* ItemUser */);
	void __UseItem_Implementation(/* const ABaseEntity* ItemUser */);

	UFUNCTION(DisplayName = "UseItem", Category = "Operations", NetMulticast, Reliable)
	void __CallClientOnUse();
	void __CallClientOnUse_Implementation();

	FAddItemOperationResult __SetCurrentAmount(const int32 NewAmount, const bool Additive = true, const bool Force = false);

	bool __CanBeUsed_Internal() const;

private:

	UPROPERTY(DisplayName = "CurrentAmount", Category = "Stats", VisibleAnywhere, BlueprintGetter = GetCurrentAmount, Replicated)
	int32 m__CurrentAmount = 0;

	UPROPERTY(DisplayName = "ItemID", Category = "Info", VisibleAnywhere, Replicated)
	FBaseID m__ItemID;

	UPROPERTY(DisplayName = "OwningComponent", Category = "Instances", BlueprintGetter = GetOwningInventory)
	UItemInventoryComponent* m__OwningInventory;
};
