// Copyright MikeSMediaStudios™ 2021


#include "Systems/Base/BaseGameInstanceBPSubsystem.h"

void UBaseGameInstanceBPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnInitialize();
}

void UBaseGameInstanceBPSubsystem::Deinitialize()
{
	OnDeinitialize();
	Super::Deinitialize();
}

bool UBaseGameInstanceBPSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return ShouldCreate();
}


bool UBaseGameInstanceBPSubsystem::ShouldCreate_Implementation() const
{
	return false;
}