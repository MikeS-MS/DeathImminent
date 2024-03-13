#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Utilities/GeneralStructs.h"
#include "ItemStructs.generated.h"

class ABaseItem;
class UItemInventoryComponent;

USTRUCT(BlueprintType)
struct DEATHIMMINENT_API FItemInformation
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name = FText::FromString("");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = true))
	FText Description = FText::FromString("");


	FItemInformation()
	{

	}
};

struct FAddItemOperationResult
{
	bool Success = false;
	int32 AmountLeft = 0;
};

USTRUCT(BlueprintType)
struct FItemSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Index = -1;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FBaseID ItemID;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Amount = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UItemInventoryComponent* OwningInventory = nullptr;


	FItemSnapshot()
	{

	}

	FItemSnapshot(UItemInventoryComponent* InOwningComponent, const FBaseID& InItemID, const int32 InAmount, const int32 InItemIndex)
	:
	Index(InItemIndex),
	ItemID(InItemID),
	Amount(InAmount),
	OwningInventory(InOwningComponent)
	{

	}
};