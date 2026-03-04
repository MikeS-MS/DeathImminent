#include "Mods/ModContent.h"
#include "Systems/ContentManager.h"


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

const FGuid& UModContent::GetGuid() const
{
	return m__Guid;
}

const UModContent* UModContent::GetInstance() const
{
	return GEngine->GetWorld()->GetGameInstance()->GetSubsystem<UContentManager>()->GetModInstance(this->GetClass());
}
