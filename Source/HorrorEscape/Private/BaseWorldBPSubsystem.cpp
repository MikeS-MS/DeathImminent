// Copyright MikeSMediaStudios™ 2021


#include "BaseWorldBPSubsystem.h"

void UBaseWorldBPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnInitialize();
}

//void UBaseWorldBPSubsystem::Deinitialize()
//{
//	OnDeinitialize();
//}

bool UBaseWorldBPSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return ShouldCreate();
}

//void UBaseWorldBPSubsystem::OnDeinitialize_Implementation()
//{
//
//}

bool UBaseWorldBPSubsystem::ShouldCreate_Implementation() const
{
	return false;
}
