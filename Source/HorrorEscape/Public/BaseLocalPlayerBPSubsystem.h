// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "BaseLocalPlayerBPSubsystem.generated.h"

//UCLASS()
//class ULocalPlayer : public UObject;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class HORRORESCAPE_API UBaseLocalPlayerBPSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize();
	//UFUNCTION(BlueprintImplementableEvent)
	//void OnDeinitialize();
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldCreate() const;
};
