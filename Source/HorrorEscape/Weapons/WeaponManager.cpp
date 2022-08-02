// Copyright MikeSMediaStudios™ 2021

#include "WeaponManager.h"

void UWeaponManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UWeaponManager::LoadStats(TMap<FWeaponPartName, FWeaponPartStats> Stats)
{
	WeaponPartStats = Stats;

	TArray<FWeaponPartName> Keys;
	Stats.GetKeys(Keys);

	if (Keys.Num() > 0)
	{
		UE_LOG(WeaponManager, Warning, TEXT("Loaded stats."));
		WeaponPartStats.GetKeys(Keys);

		for (const auto& Key : Keys)
		{
			UE_LOG(WeaponManager, Warning, TEXT("Loaded entry Name: %s Slot: %d"), *Key.Name, Key.Slot.GetValue());
		}
		OnStatsLoaded.Broadcast();
	}
	else
	{
		UE_LOG(WeaponManager, Error, TEXT("Could not load stats."));
	}
}

FWeaponPartStats UWeaponManager::GetStatsForPart(FWeaponPartName PartName) const
{
	TArray<FWeaponPartName> Keys;
	TArray<FWeaponPartStats> Values;
	FWeaponPartName Key;
	FWeaponPartStats Value;

	WeaponPartStats.GetKeys(Keys);
	WeaponPartStats.GenerateValueArray(Values);

	for (int i = 0; i < Keys.Num(); i++)
	{
		Key = Keys[i];

		if (Key.Equals(PartName))
		{
			Value = Values[i];
			break;
		}
	}

	if (Key.Slot.GetValue() == EWeaponSlot::Invalid)
	{
		UE_LOG(WeaponManager, Error, TEXT("Could not find stats for Name: %s Slot: %d"), *PartName.Name, PartName.Slot.GetValue());
		return FWeaponPartStats();
	}
	UE_LOG(WeaponManager, Warning, TEXT("Found stats for Name: %s Slot: %d"), *PartName.Name, PartName.Slot.GetValue());
	return Value;
}

