// Copyright MikeSMediaStudios™

#include "Systems/ItemManager.h"
#include "Settings/DefaultDataTables.h"

UItemManager::UItemManager()
{

}

void UItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!IsValid(sm__Instance))
		sm__Instance = this;

}

void UItemManager::Deinitialize()
{
	Super::Deinitialize();
	sm__Instance = nullptr;
}

void UItemManager::_SetupData(const UDefaultDataTables* DefaultDataTables)
{
	//__LoadData(m__RarityRegistry, DefaultDataTables->Rarities.LoadSynchronous(), GAME_ID);
	__LoadData(m__ItemRegistry, DefaultDataTables->Items.LoadSynchronous(), GAME_ID);
}

ABaseItem* UItemManager::__SpawnItem(const FBaseID& ItemID, UWorld* InWorld)
{
	UWorld* World = IsValid(InWorld) ? InWorld : GetWorld();

	if (!IsValid(World))
		return nullptr;

	FItemData* ItemData = nullptr;

	if (!__ResolveData(m__ItemRegistry, ItemID, &ItemData))
		return nullptr;

	ABaseItem* Item = World->SpawnActor<ABaseItem>(ItemData->Item);
	Item->m__ItemID = ItemID;

	return Item;
}


UItemManager* UItemManager::sm__Instance = nullptr;
