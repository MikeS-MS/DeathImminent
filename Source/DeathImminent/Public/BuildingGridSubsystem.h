// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BuildingGridSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DEATHIMMINENT_API UBuildingGridSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	//~Begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//~End USubsystem interface

	//~Begin UWorldSubsystem interface
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	//~End UWorldSubsystem interface

	UFUNCTION(BlueprintGetter)
	const ABuildingGrid* GetBuildingGrid() const;

private:

	void OnActorSpawned(AActor* Actor);

private:
	UPROPERTY(BlueprintGetter = GetBuildingGrid, DisplayName = "Building Grid")
	ABuildingGrid* m_BuildingGrid;
	FDelegateHandle OnActorSpawnedDelegateHandle;
	FOnActorSpawned::FDelegate OnActorSpawnedDelegate;
};
