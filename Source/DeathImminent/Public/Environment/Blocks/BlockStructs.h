#pragma once

#include "CoreMinimal.h"
#include "BlockStructs.generated.h"


USTRUCT(BlueprintType)
struct FBlock
{
	GENERATED_BODY()
	
};

USTRUCT(BlueprintType)
struct FBlockID
{
	GENERATED_BODY()

	FBlockID()
	{
		Source = 0;
		ID = 0;
	}

	FBlockID(int32 source, int32 id)
	{
		Source = source;
		ID = id;
	}

	UPROPERTY(BlueprintReadWrite, Category="Stats", meta = (ClampMin = 0, ClampMax = 32768))
	int32 Source;
	UPROPERTY(BlueprintReadWrite, Category="Stats", meta = (ClampMin = 0, ClampMax = 32768))
	int32 ID;

	bool operator== (const FBlockID& other) const
	{
		return Equals(other);
	}

	bool Equals(const FBlockID& other) const
	{
		return (Source == other.Source) && (ID == other.ID);
	}
};


#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FBlockID& Thing);
#else // optimize by inlining in shipping and development builds
FORCEINLINE uint32 GetTypeHash(const FBlockID& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FBlockID));
	return Hash;
}
#endif