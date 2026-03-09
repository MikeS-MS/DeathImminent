// Copyright MikeSMediaStudios�

#pragma once

#include "CoreMinimal.h"
#include "Mods/ModContent.h"
#include "GeneralStructs.generated.h"

#define GAME_ID "deathimminent"

USTRUCT(BlueprintType)
struct DEATHIMMINENT_API FBaseID
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UModContent> Source;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID = -1;

	FBaseID() { }

	FString ToString() const;
	bool Equals(const FBaseID& Other) const;
	bool NotEquals(const FBaseID& Other) const;

	bool operator!=(const FBaseID& Other) const
	{
		return NotEquals(Other);
	}

	bool operator==(const FBaseID& Other) const
	{
		return Equals(Other);
	}
};
