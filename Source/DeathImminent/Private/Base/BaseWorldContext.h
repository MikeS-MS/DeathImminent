// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseWorldContext.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class DEATHIMMINENT_API UBaseWorldContext : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, BlueprintCallable)
	virtual UWorld *GetWorld() const override;
};
