// Fill out your copyright notice in the Description page of Project Settings.


#include "SlidingDoor.h"

#define OUT

void USlidingDoor::BeginPlay()
{
	Super::BeginPlay();

	this->SetUpValues(this->GetOwner());
}

void USlidingDoor::SetUpValues(AActor* Actor)
{
	if (Actor)
	{
		FVector Origin;
		FVector BoxExtent;
		
		Actor->GetActorBounds(true, OUT Origin, OUT BoxExtent);

		this->BeginPosition = Actor->GetActorLocation();
		this->EndPosition = this->BeginPosition;
		this->EndPosition.X += ((this->X.MoveBy * this->X.MoveDirection) * this->X.bCustomMoveValue) + (((BoxExtent.X * this->X.MoveDirection) * (!this->X.bCustomMoveValue)) * 2) + ((this->X.AdditiveMoveValue * this->X.bEnableAdditive) * this->X.MoveDirection);
		this->EndPosition.Y += ((this->Y.MoveBy * this->Y.MoveDirection) * this->Y.bCustomMoveValue) + (((BoxExtent.Y * this->Y.MoveDirection) * (!this->Y.bCustomMoveValue)) * 2) + ((this->Y.AdditiveMoveValue * this->Y.bEnableAdditive) * this->Y.MoveDirection);
		this->EndPosition.Z += ((this->Z.MoveBy * this->Z.MoveDirection) * this->Z.bCustomMoveValue) + (((BoxExtent.Z * this->Z.MoveDirection) * (!this->Z.bCustomMoveValue)) * 2) + ((this->Z.AdditiveMoveValue * this->Z.bEnableAdditive) * this->Z.MoveDirection);
	}
}

FVector USlidingDoor::SetUpPositions(const FMovePosition& X, const FMovePosition& Y, const FMovePosition& Z, const FVector& BoxExtent, const FVector& Position)
{
	FVector Temp = Position;

	Temp.X += ((X.MoveBy * X.MoveDirection) * X.bCustomMoveValue) + (((BoxExtent.X * X.MoveDirection) * (!X.bCustomMoveValue)) * 2) + ((X.AdditiveMoveValue * X.bEnableAdditive) * X.MoveDirection);
	Temp.Y += ((Y.MoveBy * Y.MoveDirection) * Y.bCustomMoveValue) + (((BoxExtent.Y * Y.MoveDirection) * (!Y.bCustomMoveValue)) * 2) + ((Y.AdditiveMoveValue * Y.bEnableAdditive) * Y.MoveDirection);
	Temp.Z += ((Z.MoveBy * Z.MoveDirection) * Z.bCustomMoveValue) + (((BoxExtent.Z * Z.MoveDirection) * (!Z.bCustomMoveValue)) * 2) + ((Z.AdditiveMoveValue * Z.bEnableAdditive) * Z.MoveDirection);

	return Temp;
}

void USlidingDoor::Update(float DeltaTime)
{
	this->DoorMovement(this->GetOwner(), this, DeltaTime);
}

void USlidingDoor::DoorMovement(AActor* Actor, USlidingDoor* Door, float DeltaTime)
{
	FVector Position = Actor->GetActorLocation();

	if (Door->bOpen)
	{
		if (Position != Door->EndPosition)
		{
			UpdateLocationOfSM(Position, Door->EndPosition, Door, DeltaTime);
		}
	}
	else if (!Door->bOpen)
	{
		if (Position != Door->BeginPosition)
		{
			UpdateLocationOfSM(Position, Door->BeginPosition, Door, DeltaTime);
		}
	}

	if (Position == Door->EndPosition || Position == Door->BeginPosition)
		if (!Door->bFinishedAnimation)
			Door->bFinishedAnimation = true;

	Actor->SetActorLocation(Position);
}

void USlidingDoor::UpdateLocationOfSM(FVector& StartLocation, const FVector& TargetLocation, USlidingDoor* Door, float DeltaTime)
{
	StartLocation.X = FMath::FInterpConstantTo(StartLocation.X, TargetLocation.X, DeltaTime, Door->ScalarMove);
	StartLocation.Y = FMath::FInterpConstantTo(StartLocation.Y, TargetLocation.Y, DeltaTime, Door->ScalarMove);
	StartLocation.Z = FMath::FInterpConstantTo(StartLocation.Z, TargetLocation.Z, DeltaTime, Door->ScalarMove);
}