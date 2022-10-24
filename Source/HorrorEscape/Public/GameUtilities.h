// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameUtilities.generated.h"

DECLARE_DYNAMIC_DELEGATE_FourParams(FObjectSortDelegateInt, const UObject*, A, const UObject*, B, const int, SortMethod, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_FourParams(FObjectSortDelegate, const UObject*, A, const UObject*, B, const TArray<int>&, SortMethods, bool&, Result);

UCLASS()
class HORRORESCAPE_API UGameUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortObjectItemsFirst", CompactNodeTitle = "PredicateSort", Keywords = "Array Object Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<UObject*> PredicateSortObjectItemsFirst(TArray<UObject*> Array, const FObjectSortDelegateInt& PredicateFunction, const TArray<int>& SortMethods, bool InvertResult = false);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortObjectItemsSecond", CompactNodeTitle = "PredicateSort", Keywords = "Array Object Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<UObject*> PredicateSortObjectItemsSecond(TArray<UObject*> Array, const FObjectSortDelegate& PredicateFunction, const TArray<int>& SortMethods, bool InvertResult = false);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool LesserString(const FString& A, const FString& B);
};
