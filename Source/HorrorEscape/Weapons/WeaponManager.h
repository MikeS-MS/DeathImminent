// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WeaponStucts.h"
#include "WeaponManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsLoadedDelegate);

DECLARE_LOG_CATEGORY_CLASS(WeaponManager, Warning, Warning)

UCLASS()
class HORRORESCAPE_API UWeaponManager final : public UEngineSubsystem
{
	GENERATED_BODY()
private:
	TMap<FWeaponPartName, FWeaponPartStats> WeaponPartStats;
public:
	UPROPERTY(BlueprintAssignable, Category="Dispatchers")
	FOnStatsLoadedDelegate OnStatsLoaded;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UFUNCTION(BlueprintCallable)
	void LoadStats(TMap<FWeaponPartName, FWeaponPartStats> Stats);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FWeaponPartStats GetStatsForPart(FWeaponPartName PartName) const;
};
