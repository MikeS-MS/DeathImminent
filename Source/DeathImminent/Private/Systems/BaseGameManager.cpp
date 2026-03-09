// Copyright MikeSMediaStudios�

#include "BaseGameManager.h"
#include "Systems/ContentManager.h"

void UBaseGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency(UContentManager::StaticClass());
	Super::Initialize(Collection);
}
