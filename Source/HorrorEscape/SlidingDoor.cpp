// Fill out your copyright notice in the Description page of Project Settings.


#include "SlidingDoor.h"

#define OUT

void USlidingDoor::BeginPlay()
{
	Super::BeginPlay();

	FVector Origin;
	FVector BoxExtent;
	GetOwner()->GetActorBounds(true, OUT Origin, OUT BoxExtent);
	
	this->BeginPosition = GetOwner()->GetActorLocation();			
	this->EndPosition = this->BeginPosition;
	this->EndPosition.X += ((this->X.MoveBy * this->X.MoveDirection) * this->X.bCustomMoveValue) + (((BoxExtent.X * this->X.MoveDirection) * (!this->X.bCustomMoveValue)) * 2) + ((this->X.AdditiveMoveValue * this->X.bEnableAdditive) * this->X.MoveDirection);
	this->EndPosition.Y += ((this->Y.MoveBy * this->Y.MoveDirection) * this->Y.bCustomMoveValue) + (((BoxExtent.Y * this->Y.MoveDirection) * (!this->Y.bCustomMoveValue)) * 2) + ((this->Y.AdditiveMoveValue * this->Y.bEnableAdditive) * this->Y.MoveDirection);
	this->EndPosition.Z += ((this->Z.MoveBy * this->Z.MoveDirection) * this->Z.bCustomMoveValue) + (((BoxExtent.Z * this->Z.MoveDirection) * (!this->Z.bCustomMoveValue)) * 2) + ((this->Z.AdditiveMoveValue * this->Z.bEnableAdditive) * this->Z.MoveDirection);
}

void USlidingDoor::Update(float DeltaTime)
{
	FVector Position = GetOwner()->GetActorLocation();
	
	if (this->bOpen)
	{
		if (Position != this->EndPosition)
		{
			this->MoveDoor(Position, this->EndPosition, DeltaTime);
		}
	}
	else if (!this->bOpen)
	{
		if (Position != this->BeginPosition)
		{
			this->MoveDoor(Position, this->BeginPosition, DeltaTime);
		}
	}
	
	if (Position == this->EndPosition || Position == this->BeginPosition)
		if (!this->bFinishedAnimation)
			this->bFinishedAnimation = true;
	
	GetOwner()->SetActorLocation(Position);
}

void USlidingDoor::MoveDoor(FVector& StartLocation, const FVector& TargetLocation, float DeltaTime)
{
	StartLocation.X = FMath::FInterpConstantTo(StartLocation.X, TargetLocation.X, DeltaTime, this->ScalarMove);
	StartLocation.Y = FMath::FInterpConstantTo(StartLocation.Y, TargetLocation.Y, DeltaTime, this->ScalarMove);
	StartLocation.Z = FMath::FInterpConstantTo(StartLocation.Z, TargetLocation.Z, DeltaTime, this->ScalarMove);
}