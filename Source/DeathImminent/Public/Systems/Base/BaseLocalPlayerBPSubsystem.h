// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "BaseLocalPlayerBPSubsystem.generated.h"

// UCLASS()
// class ULocalPlayer : public UObject;

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class DEATHIMMINENT_API UBaseLocalPlayerBPSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase &Collection) override;
	// virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject *Outer) const override;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	APlayerController* GetController() const
	{
		ULocalPlayer* LocalPlayer = Super::GetLocalPlayer();
		return LocalPlayer->GetPlayerController(LocalPlayer->GetWorld());
	}
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize();
	// UFUNCTION(BlueprintImplementableEvent)
	// void OnDeinitialize();
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldCreate() const;
};
