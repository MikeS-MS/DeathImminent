// Copyright MikeSMediaStudios™ 2021


#include "Systems/Base/BaseLocalPlayerBPSubsystem.h"

void UBaseLocalPlayerBPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnInitialize();
}

bool UBaseLocalPlayerBPSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return ShouldCreate();
}

bool UBaseLocalPlayerBPSubsystem::ShouldCreate_Implementation() const
{
	return false;
}