// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "Systems/BaseManager.h"
#include "Settings/DefaultDataTables.h"
#include "BaseGameManager.generated.h"

class UDefaultDataTables;

UCLASS(BlueprintType)
class UBaseGameManager : public UBaseManager
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:

	void _Setup();
	virtual void _SetupData(const UDefaultDataTables* DefaultDataTables) { }

};