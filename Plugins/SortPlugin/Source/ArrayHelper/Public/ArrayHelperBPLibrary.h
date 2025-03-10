// Copyright 2022 RLoris

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArrayHelperBPLibrary.generated.h"

class AActor;
/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

/* Sort */
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FIntSortDelegate, const int32&, A, const int32&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FFloatSortDelegate, const float&, A, const float&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FByteSortDelegate, const uint8&, A, const uint8&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FInt64SortDelegate, const int64&, A, const int64&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FStringSortDelegate, const FString&, A, const FString&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FNameSortDelegate, const FName&, A, const FName&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FVectorSortDelegate, const FVector&, A, const FVector&, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FActorSortDelegate, const AActor*, A, const AActor*, B, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FObjectSortDelegate, const UObject*, A, const UObject*, B, bool&, Result);

/* Filter */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FIntFilterDelegate, const int32&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FFloatFilterDelegate, const float&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FByteFilterDelegate, const uint8&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FInt64FilterDelegate, const int64&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FStringFilterDelegate, const FString&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FNameFilterDelegate, const FName&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FVectorFilterDelegate, const FVector&, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FActorFilterDelegate, const AActor*, Value, bool&, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FObjectFilterDelegate, const UObject*, Value, bool&, Result);

template <typename T>
class TReverseSortPredicate
{
	public:
		bool operator()(const T A, const T B) const
		{
			return A > B;
		}
};

UCLASS()
class UArrayHelperBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
public:
	/* Average */
	template <typename T> static float Average(const TArray<T>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AverageInteger", CompactNodeTitle = "Average", Keywords = "Average plugin ArrayHelper Integer", ToolTip = "Get the average of an array"), Category = "Average")
	static float AverageInteger(const TArray<int32>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AverageFloat", CompactNodeTitle = "Average", Keywords = "Average plugin ArrayHelper Float", ToolTip = "Get the average of an array"), Category = "Average")
	static float AverageFloat(const TArray<float>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AverageByte", CompactNodeTitle = "Average", Keywords = "Average plugin ArrayHelper Byte", ToolTip = "Get the average of an array"), Category = "Average")
	static float AverageByte(const TArray<uint8>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "AverageInteger64", CompactNodeTitle = "Average", Keywords = "Average plugin ArrayHelper Integer64", ToolTip = "Get the average of an array"), Category = "Average")
	static float AverageInteger64(const TArray<int64>& Array);	
	/* Minimum */
	template <typename T> static int32 Minimum(const TArray<T>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinimumIntegerIndex", CompactNodeTitle = "MinIdx", Keywords = "Minimum plugin ArrayHelper Integer Index", ToolTip = "Get the minimum value index of an array"), Category = "Minimum")
	static int32 MinimumIntegerIndex(const TArray<int32>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinimumFloatIndex", CompactNodeTitle = "MinIdx", Keywords = "Minimum plugin ArrayHelper Float Index", ToolTip = "Get the minimum value index of an array"), Category = "Minimum")
	static int32 MinimumFloatIndex(const TArray<float>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinimumByteIndex", CompactNodeTitle = "MinIdx", Keywords = "Minimum plugin ArrayHelper Byte Index", ToolTip = "Get the minimum value index of an array"), Category = "Minimum")
	static int32 MinimumByteIndex(const TArray<uint8>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinimumInteger64Index", CompactNodeTitle = "MinIdx", Keywords = "Minimum plugin ArrayHelper Integer64 Index", ToolTip = "Get the minimum value index of an array"), Category = "Minimum")
	static int32 MinimumInteger64Index(const TArray<int64>& Array);
	/* Maximum */
	template <typename T> static int32 Maximum(const TArray<T>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MaximumIntegerIndex", CompactNodeTitle = "MaxIdx", Keywords = "Maximum plugin ArrayHelper Integer Index", ToolTip = "Get the maximum value index of an array"), Category = "Maximum")
	static int32 MaximumIntegerIndex(const TArray<int32>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MaximumFloatIndex", CompactNodeTitle = "MaxIdx", Keywords = "Maximum plugin ArrayHelper Float Index", ToolTip = "Get the maximum value index of an array"), Category = "Maximum")
	static int32 MaximumFloatIndex(const TArray<float>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MaximumByteIndex", CompactNodeTitle = "MaxIdx", Keywords = "Maximum plugin ArrayHelper Byte Index", ToolTip = "Get the maximum value index of an array"), Category = "Maximum")
	static int32 MaximumByteIndex(const TArray<uint8>& Array);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MaximumInteger64Index", CompactNodeTitle = "MaxIdx", Keywords = "Maximum plugin ArrayHelper Integer64 Index", ToolTip = "Get the maximum value index of an array"), Category = "Maximum")
	static int32 MaximumInteger64Index(const TArray<int64>& Array);
	/* Normalize */
	template <typename T> static TArray<T> MinMaxNormalization(const TArray<T>& Array, T Min, T Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinMaxFloatNormalization", CompactNodeTitle = "MinMaxNormalization", Keywords = "MinMaxNormalization plugin ArrayHelper Float", ToolTip = "Normalize the number of an array between two values"), Category = "Normalization")
	static TArray<float> MinMaxFloatNormalization(const TArray<float>& Array, float Min = 0, float Max = 1);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinMaxIntegerNormalization", CompactNodeTitle = "MinMaxNormalization", Keywords = "MinMaxNormalization plugin ArrayHelper Integer", ToolTip = "Normalize the number of an array between two values"), Category = "Normalization")
	static TArray<int32> MinMaxIntegerNormalization(const TArray<int32>& Array, int32 Min = 0, int32 Max = 100);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinMaxByteNormalization", CompactNodeTitle = "MinMaxNormalization", Keywords = "MinMaxNormalization plugin ArrayHelper Byte", ToolTip = "Normalize the number of an array between two values"), Category = "Normalization")
	static TArray<uint8> MinMaxByteNormalization(const TArray<uint8>& Array, uint8 Min = 0, uint8 Max = 100);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MinMaxInteger64Normalization", CompactNodeTitle = "MinMaxNormalization", Keywords = "MinMaxNormalization plugin ArrayHelper Integer64", ToolTip = "Normalize the number of an array between two values"), Category = "Normalization")
	static TArray<int64> MinMaxInteger64Normalization(const TArray<int64>& Array, int64 Min = 0, int64 Max = 100);
	/* Reverse */
	template <typename T> static TArray<T> Reverse(const TArray<T>& Array);
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "ReverseArray", CompactNodeTitle = "Reverse", ArrayParm = "ArrayA,ArrayB", ArrayTypeDependentParams = "ArrayB", Keywords = "Reverse Plugin ArrayHelper Copy", ToolTip = "Reverse an array by copy"), Category = "Reverse")
	static void ReverseArray(const TArray<int32>& ArrayA, TArray<int32>& ArrayB);
	DECLARE_FUNCTION(execReverseArray)
	{
		// Retrieve the original array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* ArrayAAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayAProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayAProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		// Retrieve the reversed array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* ArrayBAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayBProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayBProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_FINISH;
		P_NATIVE_BEGIN;
		UArrayHelperBPLibrary::GenericArray_Reverse(ArrayAAddr, ArrayAProperty, ArrayBAddr, ArrayBProperty);
		P_NATIVE_END;
	}
	static void GenericArray_Reverse(void* ArrayA, const FArrayProperty* ArrayAProp, void* ArrayB, const FArrayProperty* ArrayBProp);
	/* Convert */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "SplitString", CompactNodeTitle = "Split", Keywords = "Convert plugin ArrayHelper String Split Explode Separator", ToolTip = "Convert a string to array"), Category = "Convert")
	static TArray<FString> SplitString(FString String, const FString& Separator, ESearchCase::Type SearchCase, bool RemoveEmptyString = true);
	template <typename T> static inline TSet<T> ToSet(const TArray<T>& Array) { return TSet<T>(Array); }
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "ToSet", CompactNodeTitle = "ToSet", ArrayParm = "ArrayA,ArrayB", ArrayTypeDependentParams = "ArrayB", Keywords = "Convert plugin ArrayHelper Object Set", ToolTip = "Convert an array to set"), Category = "Convert")
	static void ToSet(const TArray<int32>& ArrayA, TSet<int32>& ArrayB);
	DECLARE_FUNCTION(execToSet)
	{
		// Retrieve the original array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		// Retrieve the output set
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FSetProperty>(NULL);
		void* SetAddr = Stack.MostRecentPropertyAddress;
		const FSetProperty* SetProperty = CastField<FSetProperty>(Stack.MostRecentProperty);
		if (!SetProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		P_FINISH;
		P_NATIVE_BEGIN;
		UArrayHelperBPLibrary::GenericArray_ToSet(ArrayAddr, ArrayProperty, SetAddr, SetProperty);
		P_NATIVE_END;
	}
	static void GenericArray_ToSet(void* Array, const FArrayProperty* ArrayProp, void* Set, const FSetProperty* SetProp);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MakeTransformArray", CompactNodeTitle = "MakeTransformArray", Keywords = "Convert plugin ArrayHelper Make Transform", ToolTip = "Converts arrays into one transform array"), Category = "Convert")
	static bool MakeTransformArray(const TArray<FVector>& InLocations, const TArray<FRotator>& InRotators, const TArray<FVector>& InScales, TArray<FTransform>& OutTransforms);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "BreakTransformArray", CompactNodeTitle = "BreakTransformArray", Keywords = "Convert plugin ArrayHelper Break Transform", ToolTip = "Converts a transform array into arrays"), Category = "Convert")
	static void BreakTransformArray(const TArray<FTransform>& InTransforms, TArray<FVector>& OutLocations, TArray<FRotator>& OutRotators, TArray<FVector>& OutScales);
	/* Clamp */
	template <typename T> static TArray<T> Clamp(TArray<T>& Array, T Min, T Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampInteger", CompactNodeTitle = "Clamp", Keywords = "Clamp plugin ArrayHelper Integer", ToolTip = "Clamp an array"), Category = "Clamp")
	static TArray<int32> ClampInteger(TArray<int32> Array, int32 Min, int32 Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampFloat", CompactNodeTitle = "Clamp", Keywords = "Clamp plugin ArrayHelper Float", ToolTip = "Clamp an array"), Category = "Clamp")
	static TArray<float> ClampFloat(TArray<float> Array, float Min, float Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampByte", CompactNodeTitle = "Clamp", Keywords = "Clamp plugin ArrayHelper Byte", ToolTip = "Clamp an array"), Category = "Clamp")
	static TArray<uint8> ClampByte(TArray<uint8> Array, uint8 Min, uint8 Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampInteger64", CompactNodeTitle = "Clamp", Keywords = "Clamp plugin ArrayHelper Integer64", ToolTip = "Clamp an array"), Category = "Clamp")
	static TArray<int64> ClampInteger64(TArray<int64> Array, int64 Min, int64 Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampVector", CompactNodeTitle = "ClampSize", Keywords = "Clamp plugin ArrayHelper Vector Size", ToolTip = "Clamp an array"), Category = "Clamp")
	static TArray<FVector> ClampVectorSize(TArray<FVector> Array, float MinSize, float MaxSize, bool bOnly2D = false);
	/* Clamp ref */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampIntegerByRef", CompactNodeTitle = "ClampRef", Keywords = "Clamp plugin ArrayHelper Integer Reference", ToolTip = "Clamp an array"), Category = "Clamp")
	static void ClampIntegerRef(UPARAM(ref) TArray<int32>& Array, int32 Min, int32 Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampFloatByRef", CompactNodeTitle = "ClampRef", Keywords = "Clamp plugin ArrayHelper Float Reference", ToolTip = "Clamp an array"), Category = "Clamp")
	static void ClampFloatRef(UPARAM(ref) TArray<float>& Array, float Min, float Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampByteByRef", CompactNodeTitle = "ClampRef", Keywords = "Clamp plugin ArrayHelper Byte Reference", ToolTip = "Clamp an array"), Category = "Clamp")
	static void ClampByteRef(UPARAM(ref) TArray<uint8>& Array, uint8 Min, uint8 Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampInteger64ByRef", CompactNodeTitle = "ClampRef", Keywords = "Clamp plugin ArrayHelper Integer64 Reference", ToolTip = "Clamp an array"), Category = "Clamp")
	static void ClampInteger64Ref(UPARAM(ref) TArray<int64>& Array, int64 Min, int64 Max);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClampVectorRef", CompactNodeTitle = "ClampSizeRef", Keywords = "Clamp plugin ArrayHelper Vector Size Reference", ToolTip = "Clamp an array"), Category = "Clamp")
	static void ClampVectorSizeRef(UPARAM(ref) TArray<FVector>& Array, float MinSize, float MaxSize, bool bOnly2D = false);
	/* Range */
	template <typename T> static TArray<T> Extract(const TArray<T>& Array, int32 StartIndex, int32 EndIndex);
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "ExtractArray", CompactNodeTitle = "Extract", ArrayParm = "ArrayA,ArrayB", ArrayTypeDependentParams = "ArrayB", Keywords = "Extract Plugin ArrayHelper Copy Subarray", ToolTip = "Extracts a subarray from array"), Category = "Range")
	static void ExtractArray(const TArray<int32>& ArrayA, TArray<int32>& ArrayB, int32 StartIndex, int32 EndIndex);
	DECLARE_FUNCTION(execExtractArray)
	{
		// Retrieve the original array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* ArrayAAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayAProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayAProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		// Retrieve the output array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* ArrayBAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayBProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayBProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		P_GET_PROPERTY(FIntProperty, StartIndex);
		P_GET_PROPERTY(FIntProperty, EndIndex);
		P_FINISH;
		P_NATIVE_BEGIN;
		UArrayHelperBPLibrary::GenericArray_Extract(ArrayAAddr, ArrayAProperty, ArrayBAddr, ArrayBProperty, StartIndex, EndIndex);
		P_NATIVE_END;
	}
	static void GenericArray_Extract(void* ArrayA, const FArrayProperty* ArrayAProp, void* ArrayB, const FArrayProperty* ArrayBProp, int32 StartIdx, int32 EndIdx);
	/* Random */
	template <typename T> static TArray<T> Random(int32 Size, T& MinValue, T& MaxValue);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomInteger", CompactNodeTitle = "Random", Keywords = "Random plugin ArrayHelper Integer", ToolTip = "Generates a random array"), Category = "Random")
	static TArray<int32> RandomInteger(int32 Size, int32 MinValue, int32 MaxValue);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomFloat", CompactNodeTitle = "Random", Keywords = "Random plugin ArrayHelper Float", ToolTip = "Generates a random array"), Category = "Random")
	static TArray<float> RandomFloat(int32 Size, float MinValue, float MaxValue);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomVector", CompactNodeTitle = "Random", Keywords = "Random plugin ArrayHelper Vector", ToolTip = "Generates a random array"), Category = "Random")
	static TArray<FVector> RandomVector(int32 Size, FVector MinValue, FVector MaxValue);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomByte", CompactNodeTitle = "Random", Keywords = "Random plugin ArrayHelper Byte", ToolTip = "Generates a random array"), Category = "Random")
	static TArray<uint8> RandomByte(int32 Size, uint8 MinValue, uint8 MaxValue);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomBoolean", CompactNodeTitle = "Random", Keywords = "Random plugin ArrayHelper Boolean", ToolTip = "Generates a random array"), Category = "Random")
	static TArray<bool> RandomBoolean(int32 Size);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "RandomInteger64", CompactNodeTitle = "Random", Keywords = "Random plugin ArrayHelper Integer64", ToolTip = "Generates a random array"), Category = "Random")
	static TArray<int64> RandomInteger64(int32 Size, int64 MinValue, int64 MaxValue);
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "PickRandomItem", CompactNodeTitle = "PickRandom", ArrayParm = "ArrayA", ArrayTypeDependentParams = "Item", Keywords = "Random Plugin ArrayHelper Weight Pick Select", ToolTip = "Picks a random item in an array"), Category = "Random")
	static void RandomItem(const TArray<int32>& ArrayA, TArray<int32> Weights, int32& Item, int32& Index);
	DECLARE_FUNCTION(execRandomItem)
	{
		// Retrieve the original array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		// weights array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		const TArray<int32> Weights = *reinterpret_cast<TArray<int32>*>(Stack.MostRecentPropertyAddress);
		// item
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		void* ItemPtr = Stack.MostRecentPropertyAddress;
		P_GET_PROPERTY_REF(FIntProperty, Index);
		P_FINISH;
		P_NATIVE_BEGIN;
		UArrayHelperBPLibrary::GenericArray_RandomItem(ArrayAddr, ArrayProperty, Weights, ItemPtr, Index);
		P_NATIVE_END;
	}
	static void GenericArray_RandomItem(void* Array, const FArrayProperty* ArrayProp, TArray<int32> Weights, void* Item, int32& Index);
	/* Sort */
	template <typename T> static TArray<T> Sort(TArray<T>& Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortInteger", CompactNodeTitle = "Sort", Keywords = "Sort plugin ArrayHelper Integer", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
	static TArray<int32> SortInteger(TArray<int32> Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortFloat", CompactNodeTitle="Sort", Keywords = "Sort plugin ArrayHelper Float", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
	static TArray<float> SortFloat(TArray<float> Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortByte", CompactNodeTitle = "Sort", Keywords = "Sort plugin ArrayHelper Byte", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
	static TArray<uint8> SortByte(TArray<uint8> Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortInteger64", CompactNodeTitle = "Sort", Keywords = "Sort plugin ArrayHelper Integer64", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
	static TArray<int64> SortInteger64(TArray<int64> Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortString", CompactNodeTitle = "Sort", Keywords = "Sort plugin ArrayHelper String", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
	static TArray<FString> SortString(TArray<FString> Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortName", CompactNodeTitle = "Sort", Keywords = "Sort plugin ArrayHelper Name", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
	static TArray<FName> SortName(TArray<FName> Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortActor", CompactNodeTitle = "Sort", Keywords = "Sort plugin ArrayHelper Actor", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
	static TArray<AActor*> SortActor(TArray<AActor*> Array, AActor* const & Actor, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortVector", CompactNodeTitle = "Sort", Keywords = "Sort plugin ArrayHelper Vector", ToolTip = "Sort an array in ascending or descending order"), Category = "Sort")
	static TArray<FVector> SortVector(TArray<FVector> Array, FVector Origin, bool bIsAscending = true);
	/* Sort ref */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortIntegerByRef", CompactNodeTitle = "SortRef", Keywords = "Sort plugin ArrayHelper Integer Reference", ToolTip = "Sort an array in ascending or descending order by reference"), Category = "Sort")
	static void SortIntegerRef(UPARAM(ref) TArray<int32>& Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortFloatByRef", CompactNodeTitle = "SortRef", Keywords = "Sort plugin ArrayHelper Float Reference", ToolTip = "Sort an array in ascending or descending order by reference"), Category = "Sort")
	static void SortFloatRef(UPARAM(ref) TArray<float>& Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortByteByRef", CompactNodeTitle = "SortRef", Keywords = "Sort plugin ArrayHelper Byte Reference", ToolTip = "Sort an array in ascending or descending order by reference"), Category = "Sort")
	static void SortByteRef(UPARAM(ref) TArray<uint8>& Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortInteger64ByRef", CompactNodeTitle = "SortRef", Keywords = "Sort plugin ArrayHelper Integer64 Reference", ToolTip = "Sort an array in ascending or descending order by reference"), Category = "Sort")
	static void SortInteger64Ref(UPARAM(ref) TArray<int64>& Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortStringByRef", CompactNodeTitle = "SortRef", Keywords = "Sort plugin ArrayHelper String Reference", ToolTip = "Sort an array in ascending or descending order by reference"), Category = "Sort")
	static void SortStringRef(UPARAM(ref) TArray<FString>& Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortNameByRef", CompactNodeTitle = "SortRef", Keywords = "Sort plugin ArrayHelper Name Reference", ToolTip = "Sort an array in ascending or descending order by reference"), Category = "Sort")
	static void SortNameRef(UPARAM(ref) TArray<FName>& Array, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortActorByRef", CompactNodeTitle = "SortRef", Keywords = "Sort plugin ArrayHelper Actor Reference", ToolTip = "Sort an array in ascending or descending order by reference"), Category = "Sort")
	static void SortActorRef(UPARAM(ref) TArray<AActor*>& Array, AActor* const & Actor, bool bIsAscending = true);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "SortVectorByRef", CompactNodeTitle = "SortRef", Keywords = "Sort plugin ArrayHelper Vector Reference", ToolTip = "Sort an array in ascending or descending order by reference"), Category = "Sort")
	static void SortVectorRef(UPARAM(ref) TArray<FVector>& Array, FVector Origin, bool bIsAscending = true);
	/* Sort Predicate */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortInteger", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper Integer Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<int32> PredicateSortInteger(TArray<int32> Array, const FIntSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortFloat", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper Float Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<float> PredicateSortFloat(TArray<float> Array, const FFloatSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortByte", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper Byte Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<uint8> PredicateSortByte(TArray<uint8> Array, const FByteSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortInteger64", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper Integer64 Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<int64> PredicateSortInteger64(TArray<int64> Array, const FInt64SortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortString", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper String Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<FString> PredicateSortString(TArray<FString> Array, const FStringSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortName", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper Name Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<FName> PredicateSortName(TArray<FName> Array, const FNameSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortVector", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper Vector Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<FVector> PredicateSortVector(TArray<FVector> Array, const FVectorSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortActor", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper Actor Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<AActor*> PredicateSortActor(TArray<AActor*> Array, const FActorSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortObject", CompactNodeTitle = "PredicateSort", Keywords = "Sort plugin ArrayHelper Object Predicate", ToolTip = "Sort an array using a predicate"), Category = "Sort")
	static TArray<UObject*> PredicateSortObject(TArray<UObject*> Array, const FObjectSortDelegate& PredicateFunction, bool InvertResult = false);
	/* Sort Predicate ref */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortIntegerByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper Integer Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortIntegerRef(UPARAM(ref) TArray<int32>& Array, const FIntSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortFloatByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper Float Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortFloatRef(UPARAM(ref) TArray<float>& Array, const FFloatSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortByteByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper Byte Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortByteRef(UPARAM(ref) TArray<uint8>& Array, const FByteSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortInteger64ByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper Integer64 Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortInteger64Ref(UPARAM(ref) TArray<int64>& Array, const FInt64SortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortStringByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper String Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortStringRef(UPARAM(ref) TArray<FString>& Array, const FStringSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortNameByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper Name Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortNameRef(UPARAM(ref) TArray<FName>& Array, const FNameSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortVectorByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper Vector Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortVectorRef(UPARAM(ref) TArray<FVector>& Array, const FVectorSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortActorByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper Actor Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortActorRef(UPARAM(ref) TArray<AActor*>& Array, const FActorSortDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateSortObjectByRef", CompactNodeTitle = "PredicateSortRef", Keywords = "Sort plugin ArrayHelper Object Predicate Reference", ToolTip = "Sort an array using a predicate by reference"), Category = "Sort")
	static void PredicateSortObjectRef(UPARAM(ref) TArray<UObject*>& Array, const FObjectSortDelegate& PredicateFunction, bool InvertResult = false);
	/* Distance */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClosestLocation", CompactNodeTitle = "Closest", Keywords = "Vector plugin ArrayHelper Closest", ToolTip = "Get closest location to an origin"), Category = "Distance")
	static void ClosestLocation(const TArray<FVector>& Array, FVector Origin, FVector& Closest, float& Distance, int32& Index);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "FarthestLocation", CompactNodeTitle = "Farthest", Keywords = "Vector plugin ArrayHelper Farthest", ToolTip = "Get farthest location to an origin"), Category = "Distance")
	static void FarthestLocation(const TArray<FVector>& Array, FVector Origin, FVector& Farthest, float& Distance, int32& Index);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClosestActor", CompactNodeTitle = "Closest", Keywords = "Actor plugin ArrayHelper Closest", ToolTip = "Get closest actor to an origin actor"), Category = "Distance")
	static void ClosestActor(const TArray<AActor*>& Array, AActor* const & Origin, AActor*& Closest, float& Distance, int32& Index);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "FarthestActor", CompactNodeTitle = "Farthest", Keywords = "Actor plugin ArrayHelper Farthest", ToolTip = "Get farthest actor to an origin actor"), Category = "Distance")
	static void FarthestActor(const TArray<AActor*>& Array, AActor* const & Origin, AActor*& Farthest, float& Distance, int32& Index);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "ClosestTransform", CompactNodeTitle = "Closest", Keywords = "Transform plugin ArrayHelper Closest", ToolTip = "Get closest transform to an origin"), Category = "Distance")
	static void ClosestTransform(const TArray<FTransform>& InArray, FVector InOrigin, FTransform& OutClosest, float& OutDistance, int32& OutIndex);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "FarthestTransform", CompactNodeTitle = "Farthest", Keywords = "Transform plugin ArrayHelper Farthest", ToolTip = "Get farthest transform to an origin"), Category = "Distance")
	static void FarthestTransform(const TArray<FTransform>& InArray, FVector InOrigin, FTransform& OutFarthest, float& OutDistance, int32& OutIndex);
	/* Filters (regex) */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "FilterMatches", CompactNodeTitle = "Matches", Keywords = "Filter plugin ArrayHelper Matches Regex", ToolTip = "Finds matching regex expressions in array"), Category = "Filter")
	static TArray<FString> FilterMatches(const TArray<FString>& Array, const FString& Pattern, bool& bFound, TArray<int32>& Indexes);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "FilterMatch", CompactNodeTitle = "Match", Keywords = "Filter plugin ArrayHelper Match Regex", ToolTip = "Finds the first matching regex expression in array"), Category = "Filter")
	static FString FilterMatch(const TArray<FString>& Array, const FString& Pattern, bool& bFound, int32& Index);
	/* Filter Predicate */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterInteger", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper Integer Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<int32> PredicateFilterInteger(const TArray<int32>& Array, const FIntFilterDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterFloat", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper Float Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<float> PredicateFilterFloat(const TArray<float>& Array, const FFloatFilterDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterByte", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper Byte Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<uint8> PredicateFilterByte(const TArray<uint8>& Array, const FByteFilterDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterInteger64", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper Integer64 Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<int64> PredicateFilterInteger64(const TArray<int64>& Array, const FInt64FilterDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterString", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper String Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<FString> PredicateFilterString(const TArray<FString>& Array, const FStringFilterDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterName", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper Name Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<FName> PredicateFilterName(const TArray<FName>& Array, const FNameFilterDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterVector", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper Vector Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<FVector> PredicateFilterVector(const TArray<FVector>& Array, const FVectorFilterDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterActor", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper Actor Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<AActor*> PredicateFilterActor(const TArray<AActor*>& Array, const FActorFilterDelegate& PredicateFunction, bool InvertResult = false);
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "PredicateFilterObject", CompactNodeTitle = "PredicateFilter", Keywords = "Filter plugin ArrayHelper Object Predicate", ToolTip = "Filter an array using a predicate"), Category = "Filter")
	static TArray<UObject*> PredicateFilterObject(const TArray<UObject*>& Array, const FObjectFilterDelegate& PredicateFunction, bool InvertResult = false);
	/* Math */
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "ArrayAddByRef", CompactNodeTitle = "+", ArrayParm = "InArrayA", ArrayTypeDependentParams = "InItem", Keywords = "Math Plugin ArrayHelper Add +", ToolTip = "Adds an item of the same type to an array by reference"), Category = "Math")
	static void ArrayAddRef(UPARAM(ref) TArray<int32>& InArrayA, const int32& InItem, bool& OutResult);
	DECLARE_FUNCTION(execArrayAddRef)
	{
		// Retrieve the original array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		// item
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		void* ItemPtr = Stack.MostRecentPropertyAddress;
		const FProperty* ItemProperty = Stack.MostRecentProperty;
		P_GET_UBOOL_REF(OutResult)
		P_FINISH;
		P_NATIVE_BEGIN;
		UArrayHelperBPLibrary::GenericArray_MathOperation(ArrayAddr, ArrayProperty, ItemPtr, ItemProperty, OutResult, UArrayHelperBPLibrary::AddOperation);
		P_NATIVE_END;
	}
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "ArraySubtractByRef", CompactNodeTitle = "-", ArrayParm = "InArrayA", ArrayTypeDependentParams = "InItem", Keywords = "Math Plugin ArrayHelper Subtract -", ToolTip = "Subtracts an item of the same type to an array by reference"), Category = "Math")
	static void ArraySubtractRef(UPARAM(ref) TArray<int32>& InArrayA, const int32& InItem, bool& OutResult);
	DECLARE_FUNCTION(execArraySubtractRef)
	{
		// Retrieve the original array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		// item
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		void* ItemPtr = Stack.MostRecentPropertyAddress;
		const FProperty* ItemProperty = Stack.MostRecentProperty;
		P_GET_UBOOL_REF(OutResult)
		P_FINISH;
		P_NATIVE_BEGIN;
		UArrayHelperBPLibrary::GenericArray_MathOperation(ArrayAddr, ArrayProperty, ItemPtr, ItemProperty, OutResult, UArrayHelperBPLibrary::SubtractOperation);
		P_NATIVE_END;
	}
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "ArrayMultiplyByRef", CompactNodeTitle = "*", ArrayParm = "InArrayA", ArrayTypeDependentParams = "InItem", Keywords = "Math Plugin ArrayHelper Multiply *", ToolTip = "Multiplies an item of the same type to an array by reference"), Category = "Math")
	static void ArrayMultiplyRef(UPARAM(ref) TArray<int32>& InArrayA, const int32& InItem, bool& OutResult);
	DECLARE_FUNCTION(execArrayMultiplyRef)
	{
		// Retrieve the original array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		// item
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		void* ItemPtr = Stack.MostRecentPropertyAddress;
		const FProperty* ItemProperty = Stack.MostRecentProperty;
		P_GET_UBOOL_REF(OutResult)
		P_FINISH;
		P_NATIVE_BEGIN;
		UArrayHelperBPLibrary::GenericArray_MathOperation(ArrayAddr, ArrayProperty, ItemPtr, ItemProperty, OutResult, UArrayHelperBPLibrary::MultiplyOperation);
		P_NATIVE_END;
	}
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName = "ArrayDivideByRef", CompactNodeTitle = "/", ArrayParm = "InArrayA", ArrayTypeDependentParams = "InItem", Keywords = "Math Plugin ArrayHelper Divide /", ToolTip = "Divides an item of the same type to an array by reference"), Category = "Math")
	static void ArrayDivideRef(UPARAM(ref) TArray<int32>& InArrayA, const int32& InItem, bool& OutResult);
	DECLARE_FUNCTION(execArrayDivideRef)
	{
		// Retrieve the original array
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		// item
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		void* ItemPtr = Stack.MostRecentPropertyAddress;
		const FProperty* ItemProperty = Stack.MostRecentProperty;
		P_GET_UBOOL_REF(OutResult)
		P_FINISH;
		P_NATIVE_BEGIN;
		UArrayHelperBPLibrary::GenericArray_MathOperation(ArrayAddr, ArrayProperty, ItemPtr, ItemProperty, OutResult, UArrayHelperBPLibrary::DivideOperation);
		P_NATIVE_END;
	}
	static void GenericArray_MathOperation(void* InArray, const FArrayProperty* InArrayProp, void* InItem, const FProperty* InItemProp, bool& OutSuccess, const int32& InOperation);
	
protected:
	static inline constexpr int32 AddOperation = 0;
	static inline constexpr int32 SubtractOperation = 1;
	static inline constexpr int32 MultiplyOperation = 2;
	static inline constexpr int32 DivideOperation = 3;

	template <typename T>
	static TFunctionRef<T(T, T)> GetMathOperation(const int32& InOperation);
};
