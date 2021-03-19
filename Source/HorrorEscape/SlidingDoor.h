// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Door.h"
#include "SlidingDoor.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct HORRORESCAPE_API FMovePosition
{
	GENERATED_BODY()

	FMovePosition() {}
	
	/*Enable moving on this axis. (Enable editing of other values in editor only.)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bEnableMoving{ false };

	/*-1 moves the door in a negative direction on the axis, 0 means no movement and (+)1 moves it in a positive direction on the axis*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bEnableMoving", ClampMin = "-1", ClampMax = "1"))
	int MoveDirection { 0 };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bEnableMoving"))
	bool bEnableAdditive{ false };
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bEnableAdditive", ClampMin = "0"))
	float AdditiveMoveValue{ 0.f };
	
	/*Enable custom value to move the door. OVERRIDE*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bEnableMoving"))
	bool bCustomMoveValue { false };

	/*Custom value to move the door by. Default = Position +- SizeOfMesh*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "bCustomMoveValue", ClampMin = "0"))
	float MoveBy{ 0.f };
};

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HORRORESCAPE_API USlidingDoor : public UDoor
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
private:
	UFUNCTION(BlueprintCallable)
	void SetUpValues(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	static FVector SetUpPositions(const FMovePosition& X, const FMovePosition& Y, const FMovePosition& Z, const FVector& BoxExtent, const FVector& Position);
	virtual void Update(float DeltaTime) override;
	static void DoorMovement(AActor* Actor, USlidingDoor* Door, float DeltaTime);
	static void UpdateLocationOfSM(FVector& StartLocation, const FVector& TargetLocation, USlidingDoor* Door, float DeltaTime);
private:
	FVector BeginPosition;
	FVector EndPosition;

	UPROPERTY(EditAnywhere)
	FMovePosition X;

	UPROPERTY(EditAnywhere)
	FMovePosition Y;

	UPROPERTY(EditAnywhere)
	FMovePosition Z;
};
