// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComponentManipulation.h"
#include "Door.h"
#include "SlidingDoor.generated.h"

/**
 * 
 */

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
