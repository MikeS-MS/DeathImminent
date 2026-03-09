#include "Mods/ModContent.h"
#include "Systems/ContentManager.h"
#include "Utilities/GameUtilities.h"


void UModContent::_Initialize_Implementation()
{
	m__Guid = FGuid::NewGuid();
}

void UModContent::_Deinitialize_Implementation()
{

}

const FName& UModContent::GetModName() const
{
	return m_Name;
}

const UModContent* UModContent::GetInstance() const
{
	CHECK_INSTANCE_RETURN(UContentManager, ContentManager, nullptr)
	return ContentManager->GetModInstance(this->GetClass());
}
