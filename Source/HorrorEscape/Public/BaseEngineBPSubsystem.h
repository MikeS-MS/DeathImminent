// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "BaseEngineBPSubsystem.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class HORRORESCAPE_API UBaseEngineBPSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
