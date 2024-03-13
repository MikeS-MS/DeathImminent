// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Items/BaseItem.h"
#include "ItemManager.generated.h"

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Deprecated = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Redirect = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FBaseID RedirectToItem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FItemInformation ItemInfo;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<ABaseItem> Item;
};

UCLASS(BlueprintType)
class DEATHIMMINENT_API UItemManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class UItemInventoryComponent;

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	bool DoesItemExist(const FBaseID& ItemID)
	{
		FItemData* ItemData = nullptr;
		return __DoesItemExist_Internal(ItemID, &ItemData);
	}

	static UItemManager* GetInstance()
	{
		return sm__Instance;
	}

private:
	/**
	 * @brief Checks recursively if an item is valid for the given ItemID.\n
	 * If it's not loaded it will return false.\n
	 * If it hasn't been redirected it will return true.\n
	 * If it has been deprecated and not redirected it will be false.\n
	 * If it has been deprecated and redirected it will be determined by recursively checking if it has been redirected again, so it can be false if the last item at the chain is deprecated.
	 */
	bool __ResolveItemData(const FBaseID& ItemID, FItemData** ItemData);
	ABaseItem* __SpawnItem(const FBaseID& ItemID);

	bool __DoesItemExist_Internal(const FBaseID& ItemID, FItemData** OutItemData);

private:
	static UItemManager* sm__Instance;
	TMap<FString, TMap<int32, FItemData>> m__ItemRegistry;
};