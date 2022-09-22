// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ItemManagerBase.h"
#include "InstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class HORRORESCAPE_API UInstanceSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UItemManagerBase* ItemManager;
public:
	virtual void Initiliaze(FSubsystemCollectionBase& Collection);
	UFUNCTION(BlueprintCallable)
	void Load(UItemManagerBase* manager);
	UFUNCTION(BlueprintCallable)
	UItemManagerBase* GetItemManager();
};
