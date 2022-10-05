// Copyright MikeSMediaStudios™ 2021


#include "InstanceSubsystem.h"

void UInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UInstanceSubsystem::Load(UItemManagerBase* manager)
{
	ItemManager = manager;
}

UItemManagerBase* UInstanceSubsystem::GetItemManager()
{
	return ItemManager;
}
