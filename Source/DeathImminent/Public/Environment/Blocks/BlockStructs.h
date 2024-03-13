// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Utilities/GeneralStructs.h"
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
struct FBlock
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	double Fullness;

	UPROPERTY(BlueprintReadWrite)
	FBaseID BlockID;


	FBlock() : Fullness(0.0)
	{

	}

	FBlock(const FBaseID& ID, const double InFullness)
	{
		BlockID = ID;
		Fullness = InFullness;
	}

	double SetFullness(const double NewFullness, double& Overflow)
	{
		if (NewFullness > 1.0)
			Overflow = NewFullness - 1.0;
		Fullness = FMath::Clamp(NewFullness, 0.0, 1.0);
		return Fullness;
	}

	bool IsFull() const
	{
		return Fullness >= 1.0;
	}

	static const FBlock Air;
	static const FBlock Invalid;

	static const FBaseID AirID;
	static const FBaseID InvalidID;
};

inline const FBaseID FBlock::AirID = FBaseID(0);
inline const FBaseID FBlock::InvalidID = FBaseID(-1);
inline const FBlock FBlock::Air = FBlock(AirID, 0.0);
inline const FBlock FBlock::Invalid = FBlock(InvalidID, 0.0);

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBlockSizeTest, "Block Struct Size Test", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

inline bool FBlockSizeTest::RunTest(const FString& Parameters)
{
	UE_LOG(LogTemp, Error, TEXT("%lluB"), sizeof(FBlock));
	return true;
}

#endif