// Copyright MikeSMediaStudios™ 2021


#include "Systems/Base/BaseWorldBPSubsystem.h"

void UBaseWorldBPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnInitialize();
}
//
//void UBaseWorldBPSubsystem::Deinitialize()
//{
//	OnDeinitialize();
//	Super::Deinitialize();
//}

bool UBaseWorldBPSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return ShouldCreate();
}

void UBaseWorldBPSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	BeginPlay();
}

bool UBaseWorldBPSubsystem::ShouldCreate_Implementation() const
{
	return false;
}
