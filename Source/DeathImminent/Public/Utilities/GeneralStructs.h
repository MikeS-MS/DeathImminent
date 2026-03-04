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

	FBaseID()
	{
		
	}

	FString ToString() const
	{
		return "(Source: " + Source->GetDefaultObject()->GetName() + ", ID: " + FString::FromInt(ID) + ")";
	}

	static bool Equals(const FBaseID& A, const FBaseID& B)
	{
		return A == B;
	}

	bool Equals(const FBaseID& Other) const
	{
		return *this == Other;
	}

	bool operator!=(const FBaseID& Other) const
	{
		return Source.GetDefaultObject()->GetInstance()->GetGuid() != Other.Source.GetDefaultObject()->GetInstance()->GetGuid() || ID != Other.ID;
	}

	bool operator==(const FBaseID& Other) const
	{
		return Source.GetDefaultObject()->GetInstance()->GetGuid() == Other.Source.GetDefaultObject()->GetInstance()->GetGuid() && ID == Other.ID;
	}

	bool IsDefault() const
	{
		return this->ID < 0;
	}
	
	static FBaseID InvalidId;
};

inline bool IsValid(const FBaseID& IDStruct)
{
	return IDStruct.ID > -1;
}