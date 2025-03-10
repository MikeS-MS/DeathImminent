// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BlockStructs.h"
#include "BlockManager.generated.h"

UCLASS()
class DEATHIMMINENT_API UBlockManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void LoadBlocks(const UDataTable* BlockTable);

private:

	void SetupPositions();

private:

	//TMap<FSurroundingBlockStatus, FBlockPositions> m_BlockVertices;
};
