// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "UMG.h"
#include "BaseItemMenu.generated.h"

class UItemInventoryComponent;
class ABaseItem;

UCLASS(Abstract, BlueprintType, Blueprintable)
class DEATHIMMINENT_API UBaseItemMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	void Setup(UItemInventoryComponent* OwningInventory, const int32 ItemIndex);

protected:

	UPROPERTY()
	ABaseItem* m_ItemReference = nullptr;

protected:

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintProtected))
	ABaseItem* GetItemReference()
	{
		return m_ItemReference;
	}
};