#include "Utilities/GeneralStructs.h"
#include "Systems/ContentManager.h"
#include "Utilities/GameUtilities.h"

FString FBaseID::ToString() const
{
	CHECK_INSTANCE_RETURN(UContentManager, ContentManager, "")
	return "(Source: " + ContentManager->GetModInstance(Source)->GetModName().ToString() + ", ID: " + FString::FromInt(ID) + ")";
}

bool FBaseID::Equals(const FBaseID& Other) const
{
	CHECK_INSTANCE_RETURN(UContentManager, ContentManager, false)
	return ContentManager->GetModInstance(Source)->GetGuid() == ContentManager->GetModInstance(Other.Source)->GetGuid() && ID == Other.ID;
}

bool FBaseID::NotEquals(const FBaseID& Other) const
{
	CHECK_INSTANCE_RETURN(UContentManager, ContentManager, false)
	return  ContentManager->GetModInstance(Source)->GetGuid() != ContentManager->GetModInstance(Other.Source)->GetGuid() || ID != Other.ID;
}
