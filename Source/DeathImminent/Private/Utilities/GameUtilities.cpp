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

int32 UGameUtilities::PackInt16ToInt32(const int32& FirstInt16, const int32& SecondInt16)
{
	return ((static_cast<int32>(FirstInt16) << 16) | static_cast<int16>(SecondInt16));
}

void UGameUtilities::UnpackInt32ToInt16(const int32& InInt32, int32& OutFirstInt16, int32& OutSecondInt16)
{
	OutFirstInt16 = static_cast<int16>((InInt32 >> 16) & 0xffff);
	OutSecondInt16 = static_cast<int16>(InInt32 & 0xffff);
}
