// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlockStructs.generated.h"

USTRUCT(BlueprintType)
struct FBlockLocations
{
	GENERATED_BODY()

	FBlockLocations()
	{
		CachedBlockSize = 0.0f;
		BottomBackLeft = FVector::Zero();
	}

	FBlockLocations(const FVector& BottomBackLeftLocation, const float& BlockSize)
	{
		CachedBlockSize = BlockSize;
		BottomBackLeft = BottomBackLeftLocation;
	}

	FORCEINLINE FVector Center() const
	{
		return BottomBackLeft + (CachedBlockSize / 2.f);
	}

	FORCEINLINE FVector BottomBackRight() const
	{
		return FVector(BottomBackLeft.X + CachedBlockSize, BottomBackLeft.Y, BottomBackLeft.Z);
	}

	FORCEINLINE FVector BottomFrontRight() const
	{
		return FVector(BottomBackLeft.X + CachedBlockSize, BottomBackLeft.Y + CachedBlockSize, BottomBackLeft.Z);
	}

	FORCEINLINE FVector BottomFrontLeft() const
	{
		return FVector(BottomBackLeft.X, BottomBackLeft.Y + CachedBlockSize, BottomBackLeft.Z);
	}

	FORCEINLINE FVector TopBackLeft() const
	{
		return FVector(BottomBackLeft.X, BottomBackLeft.Y, BottomBackLeft.Z + CachedBlockSize);
	}

	FORCEINLINE FVector TopBackRight() const
	{
		return FVector(BottomBackLeft.X + CachedBlockSize, BottomBackLeft.Y, BottomBackLeft.Z + CachedBlockSize);
	}

	FORCEINLINE FVector TopFrontRight() const
	{
		return BottomBackLeft + CachedBlockSize;
	}

	FORCEINLINE FVector TopFrontLeft() const
	{
		return FVector(BottomBackLeft.X, BottomBackLeft.Y + CachedBlockSize, BottomBackLeft.Z + CachedBlockSize);
	}

	UPROPERTY(BlueprintReadWrite)
	float CachedBlockSize;

	UPROPERTY(BlueprintReadWrite)
	FVector BottomBackLeft;
};

UCLASS()
class DEATHIMMINENT_API UBlockLocationsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector BottomBackLeft(UPARAM(ref)const FBlockLocations& BlockLocations)
	{
		return BlockLocations.BottomBackLeft;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector BottomBackRight(UPARAM(ref)const FBlockLocations& BlockLocations)
	{
		return BlockLocations.BottomBackRight();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector BottomFrontRight(UPARAM(ref)const FBlockLocations& BlockLocations)
	{
		return BlockLocations.BottomFrontRight();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector BottomFrontLeft(UPARAM(ref) const FBlockLocations& BlockLocations)
	{
		return BlockLocations.BottomFrontLeft();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector TopBackLeft(UPARAM(ref)const FBlockLocations& BlockLocations)
	{
		return BlockLocations.TopBackLeft();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector TopBackRight(UPARAM(ref)const FBlockLocations& BlockLocations)
	{
		return BlockLocations.TopBackRight();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector TopFrontRight(UPARAM(ref)const FBlockLocations& BlockLocations)
	{
		return BlockLocations.TopFrontRight();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector TopFrontLeft(UPARAM(ref) const FBlockLocations& BlockLocations)
	{
		return BlockLocations.TopFrontLeft();
	}
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

	FBlockID(const int32& source, const int32& id)
	{
		Source = source;
		ID = id;
	}

	UPROPERTY(BlueprintReadWrite, Category="Stats", meta = (ClampMin = 0, ClampMax = 32768))
	int32 Source;
	UPROPERTY(BlueprintReadWrite, Category="Stats", meta = (ClampMin = 0, ClampMax = 32768))
	int32 ID;

	bool operator!= (const FBlockID& Other) const
	{
		return Other.Source != Source || Other.ID != ID;
	}

	bool operator== (const FBlockID& other) const
	{
		return Equals(other);
	}

	bool Equals(const FBlockID& other) const
	{
		return (Source == other.Source) && (ID == other.ID);
	}

	static const FBlockID Air;
	static const FBlockID Invalid;
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

//
//#if WITH_DEV_AUTOMATION_TESTS
//IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlockSizeTest, "Block Struct Size Test", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)
//
//inline bool FBlockSizeTest::RunTest(const FString& Parameters)
//{
//	UE_LOG(LogTemp, Error, TEXT("%lluB"), sizeof(FBlock));
//	return true;
//}
//
//#endif