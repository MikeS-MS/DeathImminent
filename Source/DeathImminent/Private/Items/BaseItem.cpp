// Copyright MikeSMediaStudios™ 2023


#include "Items/BaseItem.h"

#include "Environment/BuildingGrid/GridStructs.h"
#include "Net/UnrealNetwork.h"
#include "Items/ItemInventoryComponent.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

void ABaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool ABaseItem::CanBeUsed_Implementation()
{
	return true;
}

bool ABaseItem::IsFull()
{
	return m__CurrentAmount == m_MaxAmount;;
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ABaseItem::_OnUseItem_Server_Implementation()
{
	return true;
}

void ABaseItem::_OnUseItem_Client_Implementation()
{

}

void ABaseItem::__UseItem_Implementation(/* ABaseEntity* ItemUser */)
{
	if (/*!IsValid(ItemUser) || */!IsValid(m__OwningInventory))
	 	return;

	if (!__CanBeUsed_Internal() || !CanBeUsed())
		return;

	
	//if (m__OwningInventory != ItemUser->GetInventory())
	//{
	//	if (m__OwningInventory != ItemUser->GetTargetInventory())
	//		return;

	//	if (!m__OwningInventory->GetAllowItemUsageFromDifferentInventory())
	//		return;
	//}
	
	 
	if (!_OnUseItem_Server())
		return;


	if (mb_IsConsumable)
	{
		const FAddItemOperationResult Result = __SetCurrentAmount(-1);
		__CallClientOnUse();

		if (Result.Success && m__CurrentAmount < 1)
		{
			// this item needs to be removed from the inventory
		}
		return;
	}

	__CallClientOnUse();
}

void ABaseItem::__CallClientOnUse_Implementation()
{
	_OnUseItem_Client();
}

FAddItemOperationResult ABaseItem::__SetCurrentAmount(const int32 NewAmount, const bool Additive, const bool Force)
{
	FAddItemOperationResult Result;

	if (IsFull() && !Force)
		return Result;

	const int32 CalculatedNewAmount = m__CurrentAmount + NewAmount;
	if (Additive)
	{
		if (CalculatedNewAmount > m_MaxAmount)
		{
			Result.AmountLeft = m_MaxAmount - CalculatedNewAmount;
			m__CurrentAmount = m_MaxAmount;
		}
		else
		{
			m__CurrentAmount = CalculatedNewAmount;
		}
	}
	else
	{
		if (NewAmount > m_MaxAmount)
		{
			Result.AmountLeft = m_MaxAmount - NewAmount;
			m__CurrentAmount = m_MaxAmount;
		}
		else
		{
			m__CurrentAmount = NewAmount;
		}
	}

	Result.Success = true;
	return Result;
}

bool ABaseItem::__CanBeUsed_Internal() const
{
	if (!mb_IsConsumable)
		return true;

	if (m__CurrentAmount - 1 >= 0)
		return true;

	return false;
}
