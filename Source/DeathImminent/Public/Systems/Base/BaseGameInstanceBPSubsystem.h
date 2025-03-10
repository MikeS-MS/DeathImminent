// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BaseGameInstanceBPSubsystem.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class DEATHIMMINENT_API UBaseGameInstanceBPSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase &Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject *Outer) const override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize();
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeinitialize();
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldCreate() const;
};