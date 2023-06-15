// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameUtilities.generated.h"

DECLARE_DYNAMIC_DELEGATE_FourParams(FActorSortDelegateInt, const AActor*, A, const AActor*, B, const int, SortMethod, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_FiveParams(FActorSortDelegateArray, const AActor*, A, const AActor*, B, const TArray<int>&, SortMethods, const bool, Reverse, bool&, Result);

UCLASS()
class DEATHIMMINENT_API UGameUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortActorItemsFirst", CompactNodeTitle = "PredicateSortActorOne", Keywords = "Array Actor Predicate Item First", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static void PredicateSortActorItemsFirst(UPARAM(ref) TArray<AActor*>& Array, const FActorSortDelegateInt& PredicateFunction, UPARAM(ref) const TArray<int>& SortMethods, bool InvertResult = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortActorItemsSecond", CompactNodeTitle = "PredicateSortActorTwo", Keywords = "Array Actor Predicate Item Second", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static void PredicateSortActorItemsSecond(UPARAM(ref) TArray<AActor*>& Array, const FActorSortDelegateArray& PredicateFunction, UPARAM(ref) const TArray<int>& SortMethods, bool InvertResult = false);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool LesserString(const FString& A, const FString& B);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FORCEINLINE FVector Clamp(const FVector& Current, const FVector& Min, const FVector& Max)
	{
		return FVector(
			Current.X < Min.X ? Min.X : Current.X > Max.X ? Max.X : Current.X,
			Current.Y < Min.Y ? Min.Y : Current.Y > Max.Y ? Max.Y : Current.Y,
			Current.Z < Min.Z ? Min.Z : Current.Z > Max.Z ? Max.Z : Current.Z);
	}
};