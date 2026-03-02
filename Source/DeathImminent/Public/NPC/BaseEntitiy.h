// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "BaseEntitiy.generated.h"

class UItemInventoryComponent;

UCLASS(BlueprintType, Blueprintable, Abstract)
class DEATHIMMINENT_API ABaseEntity : public AActor
{
	GENERATED_BODY()

public:

	ABaseEntity();
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	virtual void BeginPlay() override;

};
