#include "Systems/ItemManager.h"

void UItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!IsValid(sm__Instance))
		sm__Instance = this;
}

bool UItemManager::__DoesItemExist_Internal(const FBaseID& ItemID, FItemData** OutItemData)
{
	if (!m__ItemRegistry.Contains(ItemID.Source))
		return false;

	if (!m__ItemRegistry[ItemID.Source].Contains(ItemID.ID))
		return false;

	*OutItemData = &m__ItemRegistry[ItemID.Source][ItemID.ID];

	if ((*OutItemData)->Deprecated)
		return false;

	return true;
}

bool UItemManager::__ResolveItemData(const FBaseID& ItemID, FItemData** ItemData)
{
	if (__DoesItemExist_Internal(ItemID, ItemData))
	{
		if ((*ItemData)->Redirect)
			return __ResolveItemData((*ItemData)->RedirectToItem, ItemData);

		return true;
	}

	if (!ItemData)
		return false;

	if (!(*ItemData))
		return false;

	if ((*ItemData)->Redirect)
		return __ResolveItemData((*ItemData)->RedirectToItem, ItemData);

	return false;
}

ABaseItem* UItemManager::__SpawnItem(const FBaseID& ItemID)
{
	FItemData* ItemData = nullptr;

	if (!__ResolveItemData(ItemID, &ItemData))
		return nullptr;

	UWorld* World = GetWorld();
	if (!IsValid(World))
		return nullptr;

	ABaseItem* Item = World->SpawnActor<ABaseItem>(ItemData->Item->GetClass());
	Item->m__ItemID = ItemID;

	return Item;
}


UItemManager* UItemManager::sm__Instance = nullptr;
