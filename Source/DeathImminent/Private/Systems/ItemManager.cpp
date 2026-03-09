// Copyright MikeSMediaStudios�

#include "Systems/ItemManager.h"

#include "Mods/DeathImminentContent.h"
#include "Settings/DefaultDataTables.h"
#include "Systems/ContentManager.h"
#include "Utilities/GameUtilities.h"

UItemManager::UItemManager()
{

}

void UItemManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	sm__Instance = this;
	Collection.InitializeDependency(UContentManager::StaticClass());
	__Setup();
}

void UItemManager::Deinitialize()
{
	Super::Deinitialize();
	sm__Instance = nullptr;
}

void UItemManager::__Setup()
{
	CHECK_INSTANCE(UContentManager, ContentManager)
	
	for (auto& ModData : ContentManager->GetLoadedMods())
	{
		UModContent* Mod = ModData.Value;
		for (auto& DataTable : Mod->GetItemsDataTable())
			__LoadData(m__ItemRegistry, DataTable.LoadSynchronous(), Mod->GetGuid());
	}
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
