// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BaseWorldBPSubsystem.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class DEATHIMMINENT_API UBaseWorldBPSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase &Collection) override;
	//virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject *Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void BeginPlay();
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize();
	//UFUNCTION(BlueprintImplementableEvent)
	//void OnDeinitialize();
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldCreate() const;
};
