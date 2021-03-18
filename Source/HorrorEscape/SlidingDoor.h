// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "SlidingDoor.generated.h"

/**
 * 
 */

USTRUCT()
struct HORRORESCAPE_API FMovePosition
{
	GENERATED_BODY()

	FMovePosition() {}
	
	/*Enable moving on this axis. (Enable editing of other values in editor only.)*/
	UPROPERTY(EditAnywhere)
	bool bEnableMoving{ false };

	/*-1 moves the door in a negative direction on the axis, 0 means no movement and (+)1 moves it in a positive direction on the axis*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bEnableMoving", ClampMin = "-1", ClampMax = "1"))
	int8 MoveDirection { 0 };

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bEnableMoving"))
	bool bEnableAdditive{ false };
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bEnableAdditive", ClampMin = "0"))
	float AdditiveMoveValue{ 0.f };
	
	/*Enable custom value to move the door. OVERRIDE*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bEnableMoving"))
	bool bCustomMoveValue { false };

	/*Custom value to move the door by. Default = Position +- SizeOfMesh*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCustomMoveValue", ClampMin = "0"))
	float MoveBy{ 0.f };
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HORRORESCAPE_API USlidingDoor : public UDoor
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;
private:
	virtual void Update(float DeltaTime) override;
	void MoveDoor(FVector& StartLocation, const FVector& TargetLocation, float DeltaTime);
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
