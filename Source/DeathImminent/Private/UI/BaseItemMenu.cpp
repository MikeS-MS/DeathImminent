#include "BaseItemMenu.h"
#include "Items/ItemInventoryComponent.h"

void UBaseItemMenu::Setup(UItemInventoryComponent* OwningInventory, const int32 ItemIndex)
{
	if (!IsValid(OwningInventory))
		return;

	if (!OwningInventory->m__Items.IsValidIndex(ItemIndex))
		return;

	m_ItemReference = OwningInventory->m__Items[ItemIndex];
}
