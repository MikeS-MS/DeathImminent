#pragma once

#include "CoreMinimal.h"
#include "GeneralStructs.generated.h"

#define GAME_ID_NAME "deathimminent"

USTRUCT(BlueprintType)
struct DEATHIMMINENT_API FBaseID
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Source = GAME_ID_NAME;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID = 0;

	FBaseID() : FBaseID(GAME_ID_NAME, 0)
	{
		
	}

	FBaseID(const int32 InID)
	{
		ID = InID;
	}

	FBaseID(const FString& InSource, const int32 InID)
	{
		Source = InSource;
		ID = InID;
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
		return Source != Other.Source || ID != Other.ID;
	}

	bool operator==(const FBaseID& Other) const
	{
		return Source == Other.Source && ID == Other.ID;
	}

	static const FBaseID Default;
};

inline bool IsValid(const FBaseID& IDStruct)
{
	return IDStruct.ID > -1;
}