// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "GeneralStructs.generated.h"

#define GAME_ID "deathimminent"

USTRUCT(BlueprintType)
struct DEATHIMMINENT_API FBaseID
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Source = GAME_ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID = 0;

	FBaseID()
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

	FString ToString() const
	{
		return "(Source: " + Source + " ID: " + FString::FromInt(ID) + ")";
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

	bool IsDefault() const
	{
		return *this == Default;
	}

	static const FBaseID Default;
};

inline bool IsValid(const FBaseID& IDStruct)
{
	return IDStruct.ID > -1;
}