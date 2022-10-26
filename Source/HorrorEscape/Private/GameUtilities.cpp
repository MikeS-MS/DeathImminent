// Copyright MikeSMediaStudios™ 2021


#include "GameUtilities.h"

TArray<UObject*> UGameUtilities::PredicateSortObjectItemsFirst(TArray<UObject*> Array, const FObjectSortDelegateInt& PredicateFunction, const TArray<int>& SortMethods, bool InvertResult)
{
	Array.Sort([PredicateFunction, SortMethods, InvertResult](UObject& A, UObject& B)
		{
			bool Result;
			PredicateFunction.ExecuteIfBound(&A, &B, SortMethods[0], Result);
			return InvertResult ? !Result : Result;
		});
	return Array;
}

TArray<UObject*> UGameUtilities::PredicateSortObjectItemsSecond(TArray<UObject*> Array, const FObjectSortDelegate& PredicateFunction, const TArray<int>& SortMethods, bool InvertResult)
{
	Array.Sort([PredicateFunction, SortMethods, InvertResult](UObject& A, UObject& B)
		{
			bool Result;
			PredicateFunction.ExecuteIfBound(&A, &B, SortMethods, InvertResult, Result);
			return Result;
		});
	return Array;
}

bool UGameUtilities::LesserString(const FString& A, const FString& B)
{
	return A < B;
}
