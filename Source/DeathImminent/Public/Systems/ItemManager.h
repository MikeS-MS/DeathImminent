// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "Systems/BaseGameManager.h"
#include "Items/BaseItem.h"
#include "UI/BaseItemMenu.h"
#include "ItemManager.generated.h"

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FBaseDataInformation BaseDataInformation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FItemInformation ItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABaseItem> Item;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UBaseItemMenu> ItemMenuWidget;
};

UCLASS(BlueprintType)
class DEATHIMMINENT_API UItemManager : public UBaseGameManager
{
	GENERATED_BODY()

	friend class UItemInventoryComponent;
	friend class FInventoryAddDropMoveItemTest;

public:

	UItemManager();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	bool IsItemValid(const FBaseID& ItemID, FBaseID& ReplacementID)
	{
		FItemData* ItemData = nullptr;

		if (!__ResolveData(m__ItemRegistry, ItemID, &ItemData))
			return false;

		if (!ItemData->BaseDataInformation.Redirect)
			return true;

		ReplacementID = ItemData->BaseDataInformation.RedirectTo;
		return true;
	}

	static UItemManager* GetInstance()
	{
		return sm__Instance;
	}

protected:

	virtual void _SetupData(const UDefaultDataTables* DefaultDataTables) override;

private:

	ABaseItem* __SpawnItem(const FBaseID& ItemID, UWorld* InWorld);

private:

	static UItemManager* sm__Instance;

	DataMap<FItemData> m__ItemRegistry;
};