// Copyright MikeSMediaStudios� 2023

#pragma once

#include "CoreMinimal.h"
#include "Utilities/GeneralStructs.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlockStructs.generated.h"

USTRUCT(BlueprintType)
struct FBlockLocations
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float CachedBlockSize;

	UPROPERTY(BlueprintReadWrite)
	FVector BottomBackLeft;


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

	FVector operator[](const int32 Index) const
	{
		switch (Index)
		{
		default:
			return BottomBackLeft;
		case 0:
			return BottomBackLeft;
		case 1:
			return BottomBackRight();
		case 2:
			return BottomFrontRight();
		case 3:
			return BottomFrontLeft();
		case 4:
			return TopBackLeft();
		case 5:
			return TopBackRight();
		case 6:
			return TopFrontRight();
		case 7:
			return TopFrontLeft();
		}
	}
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

class AChunk;

USTRUCT(BlueprintType)
struct FBlock
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Fullness = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 LocalBlockID = -1;

	FBlock() 
	{

	}

	FBlock(const int32 ID, const int32 InFullness, const int32& AirBlockID)
	{
		LocalBlockID = ID;
		Fullness = FMath::Clamp(InFullness, 0, 100);
		SetIsAir(AirBlockID);
	}
	
	void SetBlockID(const int32& BlockID, const int32& AirBlockID, bool CheckFullness)
	{
		LocalBlockID = BlockID;
		SetIsAir(AirBlockID, CheckFullness);
	}
	
	/**
	 * @return The new fullness.
	 */
	int32 SetFullness(const int32 NewFullness, int32& Overflow, const int32& AirBlockID)
	{
		if (NewFullness > 100)
			Overflow = NewFullness - 100;
		Fullness = FMath::Clamp(NewFullness, 0, 100);

		SetIsAir(AirBlockID);

		return Fullness;
	}
	
	FBaseID ToRealID(const AChunk* Chunk) const;
	
	float FullnessPercentage() const
	{
		if (Fullness <= 0)
			return 0.0f;
		return static_cast<float>(Fullness) * 0.01f;
	}	
	
	float Density() const
	{
		return FullnessPercentage();
	}

	bool IsFull() const
	{
		return Fullness >= 100;
	}
	
	bool IsAir() const
	{
		return IsAirCached;
	}
	
private:
	
	void SetIsAir(const int32& AirBlockID, bool CheckFullness = true)
	{
		if ((CheckFullness && Fullness <= 0) || LocalBlockID == AirBlockID)
		{
			IsAirCached = true;
			LocalBlockID = AirBlockID;
		}
		else
			IsAirCached = false;
	}
	
private:
	
	bool IsAirCached = false;
	
};

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