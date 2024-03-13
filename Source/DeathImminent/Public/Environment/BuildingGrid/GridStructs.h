#pragma once

#include "CoreMinimal.h"
#include "Utilities/GeneralStructs.h"
#include "GridStructs.generated.h"

USTRUCT(BlueprintType)
struct FSetBlockOperationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool Completed = false;

	UPROPERTY(BlueprintReadWrite)
	double BlockFullnessPreChange = 0.0;

	UPROPERTY(BlueprintReadWrite)
	double BlockFullnessPostChange = 0.0;

	UPROPERTY(BlueprintReadWrite)
	double LeftOverFullness = 0.0;

	UPROPERTY(BlueprintReadWrite)
	FBaseID BlockIDPreChange;

	UPROPERTY(BlueprintReadWrite)
	FBaseID BlockIDPostChange;
};