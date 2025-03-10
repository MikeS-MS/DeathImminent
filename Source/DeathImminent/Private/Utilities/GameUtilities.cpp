// Copyright MikeSMediaStudios™ 2021


#include "Utilities/GameUtilities.h"

void UGameUtilities::PredicateSortActorItemsFirst(TArray<AActor*>& Array, const FActorSortDelegateInt& PredicateFunction, const TArray<int>& SortMethods, bool InvertResult)
{
	Array.Sort([PredicateFunction, SortMethods, InvertResult](const AActor& A, const AActor& B)
		{
			bool Result;
			PredicateFunction.ExecuteIfBound(&A, &B, SortMethods[0], Result);
			return InvertResult ? !Result : Result;
		});
}

void UGameUtilities::PredicateSortActorItemsSecond(TArray<AActor*>& Array, const FActorSortDelegateArray& PredicateFunction, const TArray<int>& SortMethods, bool InvertResult)
{
	Array.Sort([PredicateFunction, SortMethods, InvertResult](const AActor& A, const AActor& B)
		{
			bool Result;
			PredicateFunction.ExecuteIfBound(&A, &B, SortMethods, InvertResult, Result);
			return Result;
		});
}

bool UGameUtilities::LesserString(const FString& A, const FString& B)
{
	return A < B;
}