// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Environment/BuildingGrid/BuildingGrid.h"
#include "BuildingGridSubsystem.generated.h"


UCLASS()
class DEATHIMMINENT_API UBuildingGridSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	//~Begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End USubsystem interface

	//~Begin UWorldSubsystem interface
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	//~End UWorldSubsystem interface

	/**
	 * @brief Be careful calling this as it may return a nullptr.
	 * @warning For Blueprints use only!
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ABuildingGrid* GetBuildingGrid() const
	{
		return sm__BuildingGrid;
	}

	/**
	 * @brief Be careful calling this as it may return a nullptr.
	 * @warning For C++ use only!
	 */
	static ABuildingGrid* GetBuildingGridCPP()
	{
		return sm__BuildingGrid;
	}
private:

	void __OnActorSpawned(AActor* Actor);

private:

	static ABuildingGrid* sm__BuildingGrid;
	FDelegateHandle m__OnActorSpawnedDelegateHandle;
	FOnActorSpawned::FDelegate m__OnActorSpawnedDelegate;
};
