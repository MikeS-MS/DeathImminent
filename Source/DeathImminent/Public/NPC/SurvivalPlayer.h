// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "BaseEntitiy.h"
#include "SurvivalPlayer.generated.h"

class UItemInventoryComponent;

UCLASS(BlueprintType, Blueprintable, ClassGroup = Custom)
class DEATHIMMINENT_API ASurvivalPlayer : public ABaseEntity
{
	GENERATED_BODY()

public:

	ASurvivalPlayer();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintGetter)
	UItemInventoryComponent* GetItemInventoryComponent()
	{
		return m_ItemInventoryComponent;
	}

protected:

	virtual void BeginPlay() override;

protected:

	UPROPERTY(DisplayName = "ItemInventory", Category = "Instances", BlueprintGetter = GetItemInventoryComponent, Replicated)
	UItemInventoryComponent* m_ItemInventoryComponent;
};