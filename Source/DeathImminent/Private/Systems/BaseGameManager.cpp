// Copyright MikeSMediaStudios™

#include "BaseGameManager.h"

void UBaseGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	_Setup();
}

void UBaseGameManager::_Setup()
{
	const UDefaultDataTables* DefaultDataTables = GetDefault<UDefaultDataTables>();

	if (!IsValid(DefaultDataTables))
		return;

	_SetupData(DefaultDataTables);
}
