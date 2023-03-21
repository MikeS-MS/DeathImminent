// Copyright 2022 RLoris

#include "ArrayHelperBPLibrary.h"
#include "GameFramework/Actor.h"
#include "Internationalization/Regex.h"

UArrayHelperBPLibrary::UArrayHelperBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

template <typename T> TArray<T> UArrayHelperBPLibrary::Sort(TArray<T>& Array, bool bIsAscending)
{
	if (bIsAscending)
	{
		Array.Sort();
	}
	else
	{
		Array.Sort(TReverseSortPredicate<T>());
	}
	return Array;
}

template <typename T>
TFunctionRef<T(T, T)> UArrayHelperBPLibrary::GetMathOperation(const int32& InOperation)
{
	switch (InOperation)
	{
		default:
		case UArrayHelperBPLibrary::AddOperation:
			return [](T A, T B)->T
			{
				return A + B;			
			};
			break;
		case UArrayHelperBPLibrary::SubtractOperation:
			return [](T A, T B)->T
			{
				return A - B;			
			};
			break;
		case UArrayHelperBPLibrary::MultiplyOperation:
			return [](T A, T B)->T
			{
				return A * B;			
			};
			break;
		case UArrayHelperBPLibrary::DivideOperation:
			return [](T A, T B)->T
			{
				return A / B;			
			};
			break;
	}
}

void UArrayHelperBPLibrary::GenericArray_RandomItem(void* Array, const FArrayProperty* ArrayProp, TArray<int32> Weights, void* Item, int32& OutIndex)
{
	OutIndex = INDEX_NONE;
	if (Array)
	{
		uint32 Total = 0;
		for (int32 i = 0; i < Weights.Num(); i++)
		{
			Total += FMath::Abs(Weights[i]);
			Weights[i] = Total;
		}

		FScriptArrayHelper ArrayHelper(ArrayProp, Array);
		const FProperty* InnerProp = ArrayProp->Inner;

		if (ArrayHelper.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Accessing random item from empty array"));
			InnerProp->InitializeValue(Item);
			return;
		}

		if (Weights.Num() != ArrayHelper.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("Weights array %i should have same length as array %i"), Weights.Num(), ArrayHelper.Num());
			InnerProp->InitializeValue(Item);
			return;
		}

		const int32 Val = FMath::RandRange(1, Total);

		for (int32 i = 0; i < Weights.Num(); i++)
		{
			if (Weights[i] >= Val)
			{
				OutIndex = i;
				break;
			}
		}
		InnerProp->CopySingleValueToScriptVM(Item, ArrayHelper.GetRawPtr(OutIndex));
	}
}

TArray<int32> UArrayHelperBPLibrary::SortInteger(TArray<int32> Array, bool bIsAscending)
{	
	return Sort<int32>(Array, bIsAscending);
}

TArray<float> UArrayHelperBPLibrary::SortFloat(TArray<float> Array, bool bIsAscending)
{
	return Sort<float>(Array, bIsAscending);
}

TArray<uint8> UArrayHelperBPLibrary::SortByte(TArray<uint8> Array, bool bIsAscending)
{
	return Sort<uint8>(Array, bIsAscending);
}

TArray<int64> UArrayHelperBPLibrary::SortInteger64(TArray<int64> Array, bool bIsAscending)
{
	return Sort<int64>(Array, bIsAscending);
}

TArray<FString> UArrayHelperBPLibrary::SortString(TArray<FString> Array, bool bIsAscending)
{
	return Sort<FString>(Array, bIsAscending);
}

TArray<FName> UArrayHelperBPLibrary::SortName(TArray<FName> Array, bool bIsAscending)
{
	// DO NOT USE FOR FNAME -> return Sort<FName>(Array, bIsAscending)
	if (bIsAscending)
	{
		Array.Sort([](const FName& A, const FName& B) {
			return A.LexicalLess(B);
		});
	}
	else
	{
		Array.Sort([](const FName& A, const FName& B) {
			return B.LexicalLess(A);
		});
	}
	return Array;
}

TArray<AActor*> UArrayHelperBPLibrary::SortActor(TArray<AActor*> Array, AActor* const & Actor, bool bIsAscending)
{
	if (Actor != nullptr)
	{
		if (bIsAscending)
		{
			Array.Sort([Actor](const AActor& A, const AActor& B) {
				return (&A)->GetDistanceTo(Actor) < (&B)->GetDistanceTo(Actor);
			});
		}
		else
		{
			Array.Sort([Actor](const AActor& A, const AActor& B) {
				return (&A)->GetDistanceTo(Actor) > (&B)->GetDistanceTo(Actor);
			});
		}
	}
	return Array;
}

TArray<FVector> UArrayHelperBPLibrary::SortVector(TArray<FVector> Array, FVector Origin, bool bIsAscending)
{
	if (bIsAscending)
	{
		Array.Sort([Origin](const FVector& A, const FVector& B) {
			return FVector::Distance(A, Origin) < FVector::Distance(B, Origin);
		});
	}
	else
	{
		Array.Sort([Origin](const FVector& A, const FVector& B) {
			return FVector::Distance(A, Origin) > FVector::Distance(B, Origin);
		});
	}
	return Array;
}

void UArrayHelperBPLibrary::ClosestLocation(const TArray<FVector>& Array, FVector Origin, FVector& ClosestVector, float& Distance, int32& Index)
{
	float ClosestDistance = 0;
	Index = -1;
	for(int i = 0; i < Array.Num(); i++) 
	{
		const FVector& Vector = Array[i];
		Distance = FVector::Distance(Origin, Vector);
		if (i == 0 || Distance < ClosestDistance) 
		{
			Index = i;
			ClosestDistance = Distance;
			ClosestVector = Vector;
		}
	}
	Distance = ClosestDistance;
}

void UArrayHelperBPLibrary::FarthestLocation(const TArray<FVector>& Array, FVector Origin, FVector& FarthestVector, float& Distance, int32& Index)
{
	float FarthestDistance = 0;
	Index = -1;
	for (int i = 0; i < Array.Num(); i++)
	{
		const FVector& Vector = Array[i];
		Distance = FVector::Distance(Origin, Vector);
		if (i == 0 || Distance > FarthestDistance)
		{
			Index = i;
			FarthestDistance = Distance;
			FarthestVector = Vector;
		}
	}
	Distance = FarthestDistance;
}

void UArrayHelperBPLibrary::ClosestActor(const TArray<AActor*>& Array, AActor* const & Origin, AActor*& Closest, float& Distance, int32& Index)
{
	float ClosestDistance = 0;
	Index = -1;
	if (Origin == nullptr)
	{
		return;
	}
	for (int i = 0; i < Array.Num(); i++)
	{
		AActor* Actor = Array[i];
		Distance = Origin->GetDistanceTo(Actor);
		if (i == 0 || Distance < ClosestDistance)
		{
			Index = i;
			ClosestDistance = Distance;
			Closest = Actor;
		}
	}
	Distance = ClosestDistance;
}

void UArrayHelperBPLibrary::FarthestActor(const TArray<AActor*>& Array, AActor* const & Origin, AActor*& Farthest, float& Distance, int32& Index)
{
	float FarthestDistance = 0;
	Index = -1;
	if (Origin == nullptr)
	{
		return;
	}
	for (int i = 0; i < Array.Num(); i++)
	{
		AActor* Actor = Array[i];
		Distance = Origin->GetDistanceTo(Actor);
		if (i == 0 || Distance > FarthestDistance)
		{
			Index = i;
			FarthestDistance = Distance;
			Farthest = Actor;
		}
	}
	Distance = FarthestDistance;
}

void UArrayHelperBPLibrary::ClosestTransform(const TArray<FTransform>& InArray, FVector InOrigin, FTransform& OutClosest, float& OutDistance, int32& OutIndex)
{
	float ClosestDistance = 0;
	OutIndex = -1;
	for(int i = 0; i < InArray.Num(); i++) 
	{
		const FTransform& Transform = InArray[i];
		OutDistance = FVector::Distance(InOrigin, Transform.GetLocation());
		if (i == 0 || OutDistance < ClosestDistance) 
		{
			OutIndex = i;
			ClosestDistance = OutDistance;
			OutClosest = Transform;
		}
	}
	OutDistance = ClosestDistance;
}

void UArrayHelperBPLibrary::FarthestTransform(const TArray<FTransform>& InArray, FVector InOrigin, FTransform& OutFarthest, float& OutDistance, int32& OutIndex)
{
	float FarthestDistance = 0;
	OutIndex = -1;
	for (int i = 0; i < InArray.Num(); i++)
	{
		const FTransform& Transform = InArray[i];
		OutDistance = FVector::Distance(InOrigin, Transform.GetLocation());
		if (i == 0 || OutDistance > FarthestDistance)
		{
			OutIndex = i;
			FarthestDistance = OutDistance;
			OutFarthest = Transform;
		}
	}
	OutDistance = FarthestDistance;
}

TArray<int32> UArrayHelperBPLibrary::PredicateSortInteger(TArray<int32> Array, const FIntSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const int32& A, const int32& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<float> UArrayHelperBPLibrary::PredicateSortFloat(TArray<float> Array, const FFloatSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const float& A, const float& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<uint8> UArrayHelperBPLibrary::PredicateSortByte(TArray<uint8> Array, const FByteSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const uint8& A, const uint8& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<int64> UArrayHelperBPLibrary::PredicateSortInteger64(TArray<int64> Array, const FInt64SortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const int64& A, const int64& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<FString> UArrayHelperBPLibrary::PredicateSortString(TArray<FString> Array, const FStringSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const FString& A, const FString& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<FName> UArrayHelperBPLibrary::PredicateSortName(TArray<FName> Array, const FNameSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const FName& A, const FName& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<FVector> UArrayHelperBPLibrary::PredicateSortVector(TArray<FVector> Array, const FVectorSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const FVector& A, const FVector& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<AActor*> UArrayHelperBPLibrary::PredicateSortActor(TArray<AActor*> Array, const FActorSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const AActor& A, const AActor& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(&A, &B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<UObject*> UArrayHelperBPLibrary::PredicateSortObject(TArray<UObject*> Array, const FObjectSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const UObject& A, const UObject& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(&A, &B, Result);
		return InvertResult ? !Result : Result;
	});
	return Array;
}

TArray<FString> UArrayHelperBPLibrary::FilterMatches(const TArray<FString>& Array, const FString& Pattern, bool& bFound, TArray<int32>& Indexes)
{
	const FRegexPattern CustomPattern(Pattern);
	TArray<FString> Result;
	bFound = false;
	for(int32 i = 0; i < Array.Num(); i++) 
	{
		FRegexMatcher CustomMatcher(CustomPattern, Array[i]);

		if (CustomMatcher.FindNext())
		{
			bFound = true;
			Result.Add(Array[i]);
			Indexes.Add(i);
		}
	}
	return Result;
}

FString UArrayHelperBPLibrary::FilterMatch(const TArray<FString>& Array, const FString& Pattern, bool& bFound, int32& Index)
{
	const FRegexPattern CustomPattern(Pattern);
	bFound = false;
	Index = 0;
	for (FString String : Array)
	{
		FRegexMatcher CustomMatcher(CustomPattern, String);
		if (CustomMatcher.FindNext())
		{
			bFound = true;
			return String;
		}
		Index++;
	}
	Index = -1;
	return FString();
}

TArray<int32> UArrayHelperBPLibrary::PredicateFilterInteger(const TArray<int32>& Array, const FIntFilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const int32& A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

TArray<float> UArrayHelperBPLibrary::PredicateFilterFloat(const TArray<float>& Array, const FFloatFilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const float& A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

TArray<uint8> UArrayHelperBPLibrary::PredicateFilterByte(const TArray<uint8>& Array, const FByteFilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const uint8& A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

TArray<int64> UArrayHelperBPLibrary::PredicateFilterInteger64(const TArray<int64>& Array, const FInt64FilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const int64& A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

TArray<FString> UArrayHelperBPLibrary::PredicateFilterString(const TArray<FString>& Array, const FStringFilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const FString& A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

TArray<FName> UArrayHelperBPLibrary::PredicateFilterName(const TArray<FName>& Array, const FNameFilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const FName& A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

TArray<UObject*> UArrayHelperBPLibrary::PredicateFilterObject(const TArray<UObject*>& Array, const FObjectFilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const UObject* A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::GenericArray_MathOperation(void* InArray, const FArrayProperty* InArrayProp, void* InItem, const FProperty* InItemProp, bool& OutSuccess, const int32& InOperation)
{
	OutSuccess = false;
	if (InArray)
	{
		FScriptArrayHelper ArrayHelper(InArrayProp, InArray);
		const FProperty* InnerProp = InArrayProp->Inner;
		if (InnerProp->SameType(InItemProp))
		{
			if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(InItemProp))
			{
				TFunctionRef<float(float, float)> FloatOperation = GetMathOperation<float>(InOperation);
				const float Value = FloatProperty->GetFloatingPointPropertyValue(InItem);
				for (int32 Idx = 0; Idx < ArrayHelper.Num(); Idx++)
				{
					float IdxValue;
					InnerProp->CopySingleValue(&IdxValue, ArrayHelper.GetRawPtr(Idx));
					float Result = FloatOperation(IdxValue, Value);
					InnerProp->SetValue_InContainer(ArrayHelper.GetRawPtr(Idx), &Result);
				}
				OutSuccess = true;
			}
			else if (const FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(InItemProp))
			{
				TFunctionRef<double(double, double)> DoubleOperation = GetMathOperation<double>(InOperation);
				const double Value = DoubleProperty->GetFloatingPointPropertyValue(InItem);
				for (int32 Idx = 0; Idx < ArrayHelper.Num(); Idx++)
				{
					double IdxValue;
					InnerProp->CopySingleValue(&IdxValue, ArrayHelper.GetRawPtr(Idx));
					double Result = DoubleOperation(IdxValue, Value);
					InnerProp->SetValue_InContainer(ArrayHelper.GetRawPtr(Idx), &Result);
				}
				OutSuccess = true;
			}
			else if (const FIntProperty* IntProperty = CastField<FIntProperty>(InItemProp))
			{
				TFunctionRef<int32(int32, int32)> IntOperation = GetMathOperation<int32>(InOperation);
				const int32 Value = IntProperty->GetSignedIntPropertyValue(InItem);
				for (int32 Idx = 0; Idx < ArrayHelper.Num(); Idx++)
				{
					int32 IdxValue;
					InnerProp->CopySingleValue(&IdxValue, ArrayHelper.GetRawPtr(Idx));
					int32 Result = IntOperation(IdxValue, Value);
					InnerProp->SetValue_InContainer(ArrayHelper.GetRawPtr(Idx), &Result);
				}
				OutSuccess = true;
			}
			else if (const FInt64Property* Int64Property = CastField<FInt64Property>(InItemProp))
			{
				TFunctionRef<int64(int64, int64)> Int64Operation = GetMathOperation<int64>(InOperation);
				const int64 Value = Int64Property->GetSignedIntPropertyValue(InItem);
				for (int32 Idx = 0; Idx < ArrayHelper.Num(); Idx++)
				{
					int64 IdxValue;
					InnerProp->CopySingleValue(&IdxValue, ArrayHelper.GetRawPtr(Idx));
					int64 Result = Int64Operation(IdxValue, Value);
					InnerProp->SetValue_InContainer(ArrayHelper.GetRawPtr(Idx), &Result);
				}
				OutSuccess = true;
			}
			else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(InItemProp))
			{
				TFunctionRef<uint8(uint8, uint8)> ByteOperation = GetMathOperation<uint8>(InOperation);
				const uint8 Value = ByteProperty->GetUnsignedIntPropertyValue(InItem);
				for (int32 Idx = 0; Idx < ArrayHelper.Num(); Idx++)
				{
					uint8 IdxValue;
					InnerProp->CopySingleValue(&IdxValue, ArrayHelper.GetRawPtr(Idx));
					uint8 Result = ByteOperation(IdxValue, Value);
					InnerProp->SetValue_InContainer(ArrayHelper.GetRawPtr(Idx), &Result);
				}
				OutSuccess = true;
			}
			else if (const FStructProperty* StructProperty = CastField<FStructProperty>(InItemProp))
			{
				static UScriptStruct* Vector3DStruct = TBaseStructure<FVector>::Get();
				static UScriptStruct* Vector2DStruct = TBaseStructure<FVector2D>::Get();
				static UScriptStruct* Vector4DStruct = TBaseStructure<FVector4>::Get();

				if (StructProperty->Struct == Vector3DStruct)
				{
					TFunctionRef<FVector(FVector, FVector)> Vector3DOperation = GetMathOperation<FVector>(InOperation);
					FVector Value;
					StructProperty->CopySingleValue(&Value, InItem);
					for (int32 Idx = 0; Idx < ArrayHelper.Num(); Idx++)
					{
						FVector IdxValue;
						InnerProp->CopySingleValue(&IdxValue, ArrayHelper.GetRawPtr(Idx));
						FVector Result = Vector3DOperation(IdxValue, Value);
						InnerProp->SetValue_InContainer(ArrayHelper.GetRawPtr(Idx), &Result);
					}
					OutSuccess = true;
				}
				else if (StructProperty->Struct == Vector2DStruct)
				{
					TFunctionRef<FVector2D(FVector2D, FVector2D)> Vector2DOperation = GetMathOperation<FVector2D>(InOperation);
					FVector2D Value;
					StructProperty->CopySingleValue(&Value, InItem);
					for (int32 Idx = 0; Idx < ArrayHelper.Num(); Idx++)
					{
						FVector2D IdxValue;
						InnerProp->CopySingleValue(&IdxValue, ArrayHelper.GetRawPtr(Idx));
						FVector2D Result = Vector2DOperation(IdxValue, Value);
						InnerProp->SetValue_InContainer(ArrayHelper.GetRawPtr(Idx), &Result);
					}
					OutSuccess = true;
				}
				else if (StructProperty->Struct == Vector4DStruct)
				{
					TFunctionRef<FVector4(FVector4, FVector4)> Vector4DOperation = GetMathOperation<FVector4>(InOperation);
					FVector4 Value;
					StructProperty->CopySingleValue(&Value, InItem);
					for (int32 Idx = 0; Idx < ArrayHelper.Num(); Idx++)
					{
						FVector4 IdxValue;
						InnerProp->CopySingleValue(&IdxValue, ArrayHelper.GetRawPtr(Idx));
						FVector4 Result = Vector4DOperation(IdxValue, Value);
						InnerProp->SetValue_InContainer(ArrayHelper.GetRawPtr(Idx), &Result);
					}
					OutSuccess = true;
				}
			}
		}
	}
}

bool UArrayHelperBPLibrary::MakeTransformArray(const TArray<FVector>& InLocations, const TArray<FRotator>& InRotators, const TArray<FVector>& InScales, TArray<FTransform>& OutTransforms)
{
	const bool bUseLocations = !InLocations.IsEmpty();
	const bool bUseRotators = !InRotators.IsEmpty();
	const bool bUseScales = !InScales.IsEmpty();

	int32 TransformSize = 0;
	
	if (bUseLocations)
	{
		if (TransformSize != 0 && InLocations.Num() != TransformSize)
		{
			return false;
		}
		
		TransformSize = InLocations.Num();
	}
	if (bUseRotators)
	{
		if (TransformSize != 0 && InRotators.Num() != TransformSize)
		{
			return false;
		}
		
		TransformSize = InRotators.Num();
	}
	if (bUseScales)
	{
		if (TransformSize != 0 && InScales.Num() != TransformSize)
		{
			return false;
		}
		
		TransformSize = InScales.Num();
	}

	OutTransforms.Empty(TransformSize);

	if (TransformSize == 0)
	{
		return true;
	}

	for (int32 Idx = 0; Idx < TransformSize; Idx++)
	{
		FTransform NewTransform;
		if (bUseLocations)
		{
			NewTransform.SetLocation(InLocations[Idx]);
		}
		if (bUseRotators)
		{
			NewTransform.SetRotation(InRotators[Idx].Quaternion());
		}
		if (bUseScales)
		{
			NewTransform.SetScale3D(InScales[Idx]);
		}
		OutTransforms.Emplace(NewTransform);
	}

	return true;
}

void UArrayHelperBPLibrary::BreakTransformArray(const TArray<FTransform>& InTransforms, TArray<FVector>& OutLocations, TArray<FRotator>& OutRotators, TArray<FVector>& OutScales)
{
	OutLocations.Empty(InTransforms.Num());
	OutRotators.Empty(InTransforms.Num());
	OutScales.Empty(InTransforms.Num());
	
	for (const FTransform& Transform : InTransforms)
	{
		OutLocations.Add(Transform.GetLocation());
		OutRotators.Add(Transform.Rotator());
		OutScales.Add(Transform.GetScale3D());
	}
}

TArray<FVector> UArrayHelperBPLibrary::PredicateFilterVector(const TArray<FVector>& Array, const FVectorFilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const FVector& A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

TArray<AActor*> UArrayHelperBPLibrary::PredicateFilterActor(const TArray<AActor*>& Array, const FActorFilterDelegate& PredicateFunction, bool InvertResult)
{
	return Array.FilterByPredicate([PredicateFunction, InvertResult](const AActor* A) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, Result);
		return InvertResult ? !Result : Result;
	});
}

template<typename T>
int32 UArrayHelperBPLibrary::Minimum(const TArray<T>& Array)
{
	int32 Index = Array.Num() > 0 ? 0 : -1;
	for (int32 i = 1; i < Array.Num(); i++)
	{
		if (Array[i] < Array[Index]) 
		{
			Index = i;
		}
	}
	return Index;
}

template<typename T>
int32 UArrayHelperBPLibrary::Maximum(const TArray<T>& Array)
{
	int32 Index = Array.Num() > 0 ? 0 : -1;
	for (int32 i = 1; i < Array.Num(); i++)
	{
		if (Array[i] > Array[Index])
		{
			Index = i;
		}
	}
	return Index;
}

template<typename T>
TArray<T> UArrayHelperBPLibrary::MinMaxNormalization(const TArray<T>& Array, T Min, T Max)
{
	TArray<T> NormalizedArray;
	if (Array.Num() > 0) 
	{
		NormalizedArray.SetNumUninitialized(Array.Num());
		const int32 MinIdx = UArrayHelperBPLibrary::Minimum(Array);
		const int32 MaxIdx = UArrayHelperBPLibrary::Maximum(Array);
		const T MinVal = Array[MinIdx];
		const T MaxVal = Array[MaxIdx] != 0 ? Array[MaxIdx] : 1;
		for (int32 i = 0; i < Array.Num(); i++)
		{
			NormalizedArray[i] = (((Array[i] - MinVal) / (MaxVal * 1.0f)) * (Max - Min)) + Min;
		}
	}
	return NormalizedArray;
}

template <typename T> TArray<T> UArrayHelperBPLibrary::Reverse(const TArray<T>& Array)
{
	TArray<T> ReversedArray;
	for (auto i = Array.Num() - 1; i >= 0; i--)
	{
		ReversedArray.Add(Array[i]);
	}
	return ReversedArray;
}

template <typename T> float UArrayHelperBPLibrary::Average(const TArray<T>& Array)
{
	float Average = 0.0f;
	int32 Total = Array.Num();
	Total = Total > 0 ? Total : 1;

	for (const T& Value : Array)
	{
		Average += Value;
	}

	return Average / Total;
}

float UArrayHelperBPLibrary::AverageInteger(const TArray<int32>& Array)
{
	return UArrayHelperBPLibrary::Average(Array);
}

float UArrayHelperBPLibrary::AverageFloat(const TArray<float>& Array)
{
	return UArrayHelperBPLibrary::Average(Array);
}

float UArrayHelperBPLibrary::AverageByte(const TArray<uint8>& Array)
{
	return UArrayHelperBPLibrary::Average(Array);
}

float UArrayHelperBPLibrary::AverageInteger64(const TArray<int64>& Array)
{
	return UArrayHelperBPLibrary::Average(Array);
}

int32 UArrayHelperBPLibrary::MinimumIntegerIndex(const TArray<int32>& Array)
{
	return UArrayHelperBPLibrary::Minimum(Array);
}

int32 UArrayHelperBPLibrary::MinimumFloatIndex(const TArray<float>& Array)
{
	return UArrayHelperBPLibrary::Minimum(Array);
}

int32 UArrayHelperBPLibrary::MinimumByteIndex(const TArray<uint8>& Array)
{
	return UArrayHelperBPLibrary::Minimum(Array);
}

int32 UArrayHelperBPLibrary::MinimumInteger64Index(const TArray<int64>& Array)
{
	return UArrayHelperBPLibrary::Minimum(Array);
}

int32 UArrayHelperBPLibrary::MaximumIntegerIndex(const TArray<int32>& Array)
{
	return UArrayHelperBPLibrary::Maximum(Array);
}

int32 UArrayHelperBPLibrary::MaximumFloatIndex(const TArray<float>& Array)
{
	return UArrayHelperBPLibrary::Maximum(Array);
}

int32 UArrayHelperBPLibrary::MaximumByteIndex(const TArray<uint8>& Array)
{
	return UArrayHelperBPLibrary::Maximum(Array);
}

int32 UArrayHelperBPLibrary::MaximumInteger64Index(const TArray<int64>& Array)
{
	return UArrayHelperBPLibrary::Maximum(Array);
}

TArray<float> UArrayHelperBPLibrary::MinMaxFloatNormalization(const TArray<float>& Array, float Min, float Max)
{
	return UArrayHelperBPLibrary::MinMaxNormalization(Array, Min, Max);
}

TArray<int32> UArrayHelperBPLibrary::MinMaxIntegerNormalization(const TArray<int32>& Array, int32 Min, int32 Max)
{
	return UArrayHelperBPLibrary::MinMaxNormalization(Array, Min, Max);
}

TArray<uint8> UArrayHelperBPLibrary::MinMaxByteNormalization(const TArray<uint8>& Array, uint8 Min, uint8 Max)
{
	return UArrayHelperBPLibrary::MinMaxNormalization(Array, Min, Max);
}

TArray<int64> UArrayHelperBPLibrary::MinMaxInteger64Normalization(const TArray<int64>& Array, int64 Min, int64 Max)
{
	return UArrayHelperBPLibrary::MinMaxNormalization(Array, Min, Max);
}

void UArrayHelperBPLibrary::GenericArray_Reverse(void* ArrayA, const FArrayProperty* ArrayAProp, void* ArrayB, const FArrayProperty* ArrayBProp)
{
	if (ArrayA && ArrayB)
	{
		const FProperty* InnerAProp = ArrayAProp->Inner;

		if (InnerAProp->SameType(ArrayBProp->Inner))
		{
			FScriptArrayHelper ArrayAHelper(ArrayAProp, ArrayA);
			FScriptArrayHelper ArrayBHelper(ArrayBProp, ArrayB);

			ArrayBHelper.EmptyAndAddValues(ArrayAHelper.Num());
			int32 StartIdx = 0;
			for (int32 i = ArrayAHelper.Num() - 1; i >= 0; i--, StartIdx++)
			{
				InnerAProp->CopySingleValueToScriptVM(ArrayBHelper.GetRawPtr(StartIdx), ArrayAHelper.GetRawPtr(i));
			}
		}
	}
}

TArray<FString> UArrayHelperBPLibrary::SplitString(FString String, const FString& Separator, ESearchCase::Type SearchCase, bool RemoveEmptyString)
{
	FString LeftString;
	FString RightString;
	TArray<FString> Array;
	bool Split;
	do 
	{
		Split = String.Split(Separator, &LeftString, &RightString, SearchCase);
		if (Split) 
		{
			if (RemoveEmptyString)
			{
				if (!LeftString.IsEmpty())
				{
					Array.Add(LeftString);
				}
			}
			else
			{
				Array.Add(LeftString);
			}
		}
		else
		{
			if (RemoveEmptyString)
			{
				if (!String.IsEmpty())
				{
					Array.Add(String);
				}
			}
			else
			{
				Array.Add(String);
			}
		}
		String = RightString;
	} while (Split);

	return Array;
}

template <typename T> TArray<T> UArrayHelperBPLibrary::Extract(const TArray<T>& Array, int32 StartIndex, int32 EndIndex) 
{
	TArray<T> SubArray;
	if (StartIndex < EndIndex && EndIndex < Array.Num())
	{
		for (int i = StartIndex; i <= EndIndex; i++)
		{
			SubArray.Add(Array[i]);
		}
	}
	return SubArray;
}

template <typename T> TArray<T> UArrayHelperBPLibrary::Random(int32 Size, T& MinValue, T& MaxValue)
{
	TArray<T> Array;
	if (Size > 0 && MinValue <= MaxValue) {
		for (int32 i = 0; i < Size; i++)
		{
			Array.Add(FMath::RandRange(MinValue, MaxValue));
		}
	}
	return Array;
}

void UArrayHelperBPLibrary::GenericArray_Extract(void* ArrayA, const FArrayProperty* ArrayAProp, void* ArrayB, const FArrayProperty* ArrayBProp, int32 StartIdx, int32 EndIdx)
{
	if (ArrayA && ArrayB)
	{
		const FProperty* InnerAProp = ArrayAProp->Inner;

		if (InnerAProp->SameType(ArrayBProp->Inner))
		{
			FScriptArrayHelper ArrayAHelper(ArrayAProp, ArrayA);
			FScriptArrayHelper ArrayBHelper(ArrayBProp, ArrayB);

			if (StartIdx < EndIdx && EndIdx < ArrayAHelper.Num())
			{
				ArrayBHelper.EmptyAndAddValues((EndIdx - StartIdx) + 1);
				int32 CopyIdx = 0;
				for (int32 i = StartIdx; i <= EndIdx; i++, CopyIdx++)
				{
					InnerAProp->CopySingleValueToScriptVM(ArrayBHelper.GetRawPtr(CopyIdx), ArrayAHelper.GetRawPtr(i));
				}
			}

		}
	}
}

TArray<int32> UArrayHelperBPLibrary::RandomInteger(int32 Size, int32 MinValue, int32 MaxValue)
{
	return Random<int32>(Size, MinValue, MaxValue);
}

TArray<float> UArrayHelperBPLibrary::RandomFloat(int32 Size, float MinValue, float MaxValue)
{
	return Random<float>(Size, MinValue, MaxValue);
}

TArray<FVector> UArrayHelperBPLibrary::RandomVector(int32 Size, FVector MinValue, FVector MaxValue)
{
	TArray<FVector> Array;
	if (Size > 0 && MinValue.X <= MaxValue.X && MinValue.Y <= MaxValue.Y && MinValue.Z <= MaxValue.Z)
	{
		for (int i = 0; i < Size; i++)
		{
			Array.Add(FVector(FMath::FRandRange(MinValue.X, MaxValue.X), FMath::FRandRange(MinValue.Y, MaxValue.Y), FMath::FRandRange(MinValue.Z, MaxValue.Z)));
		}
	}
	return Array;
}

TArray<uint8> UArrayHelperBPLibrary::RandomByte(int32 Size, uint8 MinValue, uint8 MaxValue)
{
	return Random<uint8>(Size, MinValue, MaxValue);
}

TArray<bool> UArrayHelperBPLibrary::RandomBoolean(int32 Size)
{
	TArray<bool> Array;
	for (int i = 0; i < Size; i++)
	{
		Array.Add(FMath::RandBool());
	}
	return Array;
}

TArray<int64> UArrayHelperBPLibrary::RandomInteger64(int32 Size, int64 MinValue, int64 MaxValue)
{
	return Random<int64>(Size, MinValue, MaxValue);
}

void UArrayHelperBPLibrary::GenericArray_ToSet(void* Array, const FArrayProperty* ArrayProp, void* Set, const FSetProperty* SetProp)
{
	if (Array && Set)
	{
		FScriptSetHelper SetHelper(SetProp, Set);
		FScriptArrayHelper ArrayHelper(ArrayProp, Array);

		const int32 Size = ArrayHelper.Num();
		SetHelper.EmptyElements();
		for (int32 i = 0; i < Size; ++i)
		{
			if (ArrayHelper.IsValidIndex(i))
			{
				SetHelper.AddElement(ArrayHelper.GetRawPtr(i));
			}
		}
		SetHelper.Rehash();
	}
}

template <typename T> TArray<T> UArrayHelperBPLibrary::Clamp(TArray<T>& Array, T Min, T Max)
{
	if (Min < Max)
	{
		for (int32 i = 0; i < Array.Num(); i++)
		{
			if (Array[i] < Min)
			{
				Array[i] = Min;
			}
			else if (Array[i] > Max)
			{
				Array[i] = Max;
			}
		}
	}
	return Array;
}

TArray<int32> UArrayHelperBPLibrary::ClampInteger(TArray<int32> Array, int32 Min, int32 Max)
{
	return Clamp<int32>(Array, Min, Max);
}

TArray<float> UArrayHelperBPLibrary::ClampFloat(TArray<float> Array, float Min, float Max)
{
	return Clamp<float>(Array, Min, Max);
}

TArray<uint8> UArrayHelperBPLibrary::ClampByte(TArray<uint8> Array, uint8 Min, uint8 Max)
{
	return Clamp<uint8>(Array, Min, Max);
}

TArray<int64> UArrayHelperBPLibrary::ClampInteger64(TArray<int64> Array, int64 Min, int64 Max)
{
	return Clamp<int64>(Array, Min, Max);
}

TArray<FVector> UArrayHelperBPLibrary::ClampVectorSize(TArray<FVector> Array, float MinSize, float MaxSize, bool bOnly2D)
{
	if (MinSize < MaxSize)
	{
		for (int i = 0; i < Array.Num(); i++)
		{
			const float Size = bOnly2D ? Array[i].Size2D() : Array[i].Size();
			if (Size < MinSize || Size > MaxSize)
			{
				Array[i] = bOnly2D ? Array[i].GetClampedToSize2D(MinSize, MaxSize) : Array[i].GetClampedToSize(MinSize, MaxSize);
			}
		}
	}
	return Array;
}

void UArrayHelperBPLibrary::ClampIntegerRef(TArray<int32>& Array, int32 Min, int32 Max)
{
	Clamp<int32>(Array, Min, Max);
}

void UArrayHelperBPLibrary::ClampFloatRef(TArray<float>& Array, float Min, float Max)
{
	Clamp<float>(Array, Min, Max);
}

void UArrayHelperBPLibrary::ClampByteRef(TArray<uint8>& Array, uint8 Min, uint8 Max)
{
	Clamp<uint8>(Array, Min, Max);
}

void UArrayHelperBPLibrary::ClampInteger64Ref(TArray<int64>& Array, int64 Min, int64 Max)
{
	Clamp<int64>(Array, Min, Max);
}

void UArrayHelperBPLibrary::ClampVectorSizeRef(TArray<FVector>& Array, float MinSize, float MaxSize, bool bOnly2D)
{
	if (MinSize < MaxSize)
	{
		for (int i = 0; i < Array.Num(); i++)
		{
			const float Size = bOnly2D ? Array[i].Size2D() : Array[i].Size();
			if (Size < MinSize || Size > MaxSize)
			{
				Array[i] = bOnly2D ? Array[i].GetClampedToSize2D(MinSize, MaxSize) : Array[i].GetClampedToSize(MinSize, MaxSize);
			}
		}
	}
}

void UArrayHelperBPLibrary::SortIntegerRef(TArray<int32>& Array, bool bIsAscending)
{
	Sort<int32>(Array, bIsAscending);
}

void UArrayHelperBPLibrary::SortFloatRef(TArray<float>& Array, bool bIsAscending)
{
	Sort<float>(Array, bIsAscending);
}

void UArrayHelperBPLibrary::SortByteRef(TArray<uint8>& Array, bool bIsAscending)
{
	Sort<uint8>(Array, bIsAscending);
}

void UArrayHelperBPLibrary::SortInteger64Ref(TArray<int64>& Array, bool bIsAscending)
{
	Sort<int64>(Array, bIsAscending);
}

void UArrayHelperBPLibrary::SortStringRef(TArray<FString>& Array, bool bIsAscending)
{
	Sort<FString>(Array, bIsAscending);
}

void UArrayHelperBPLibrary::SortNameRef(TArray<FName>& Array, bool bIsAscending)
{
	// DO NOT USE FOR FNAME -> Sort<FName>(Array, bIsAscending);
	if (bIsAscending)
	{
		Array.Sort([](const FName& A, const FName& B) {
			return A.LexicalLess(B);
		});
	}
	else
	{
		Array.Sort([](const FName& A, const FName& B) {
			return B.LexicalLess(A);
		});
	}
}

void UArrayHelperBPLibrary::SortVectorRef(TArray<FVector>& Array, FVector Origin, bool bIsAscending)
{
	if (bIsAscending)
	{
		Array.Sort([Origin](const FVector& A, const FVector& B) {
			return FVector::Distance(A, Origin) < FVector::Distance(B, Origin);
		});
	}
	else
	{
		Array.Sort([Origin](const FVector& A, const FVector& B) {
			return FVector::Distance(A, Origin) > FVector::Distance(B, Origin);
		});
	}
}

void UArrayHelperBPLibrary::SortActorRef(TArray<AActor*>& Array, AActor* const & Actor, bool bIsAscending)
{
	if (Actor != nullptr)
	{
		if (bIsAscending)
		{
			Array.Sort([Actor](const AActor& A, const AActor& B) {
				return (&A)->GetDistanceTo(Actor) < (&B)->GetDistanceTo(Actor);
			});
		}
		else
		{
			Array.Sort([Actor](const AActor& A, const AActor& B) {
				return (&A)->GetDistanceTo(Actor) >  (&B)->GetDistanceTo(Actor);
			});
		}
	}
}

void UArrayHelperBPLibrary::PredicateSortIntegerRef(TArray<int32>& Array, const FIntSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const int32& A, const int32& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::PredicateSortFloatRef(TArray<float>& Array, const FFloatSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const float& A, const float& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::PredicateSortByteRef(TArray<uint8>& Array, const FByteSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const uint8& A, const uint8& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::PredicateSortInteger64Ref(TArray<int64>& Array, const FInt64SortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const int64& A, const int64& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::PredicateSortStringRef(TArray<FString>& Array, const FStringSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const FString& A, const FString& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::PredicateSortNameRef(TArray<FName>& Array, const FNameSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const FName& A, const FName& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::PredicateSortVectorRef(TArray<FVector>& Array, const FVectorSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const FVector& A, const FVector& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(A, B, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::PredicateSortActorRef(TArray<AActor*>& Array, const FActorSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const AActor& A, const AActor& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(&A, &B, Result);
		return InvertResult ? !Result : Result;
	});
}

void UArrayHelperBPLibrary::PredicateSortObjectRef(TArray<UObject*>& Array, const FObjectSortDelegate& PredicateFunction, bool InvertResult)
{
	Array.Sort([PredicateFunction, InvertResult](const UObject& A, const UObject& B) {
		bool Result;
		PredicateFunction.ExecuteIfBound(&A, &B, Result);
		return InvertResult ? !Result : Result;
	});
}
