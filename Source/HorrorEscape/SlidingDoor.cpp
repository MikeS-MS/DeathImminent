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
	this->EndPosition.X += ((this->X.MoveBy * this->X.bMoveDirection) * this->X.bCustomMoveValue) + (((BoxExtent.X * this->X.bMoveDirection) * (!this->X.bCustomMoveValue)) * 2) + ((this->X.AdditiveMoveValue * this->X.bEnableAdditive) * this->X.bMoveDirection);
	this->EndPosition.Y += ((this->Y.MoveBy * this->Y.bMoveDirection) * this->Y.bCustomMoveValue) + (((BoxExtent.Y * this->Y.bMoveDirection) * (!this->Y.bCustomMoveValue)) * 2) + ((this->Y.AdditiveMoveValue * this->Y.bEnableAdditive) * this->Y.bMoveDirection);
	this->EndPosition.Z += ((this->Z.MoveBy * this->Z.bMoveDirection) * this->Z.bCustomMoveValue) + (((BoxExtent.Z * this->Z.bMoveDirection) * (!this->Z.bCustomMoveValue)) * 2) + ((this->Z.AdditiveMoveValue * this->Z.bEnableAdditive) * this->Z.bMoveDirection);
}

void USlidingDoor::Update(float DeltaTime)
{
	FVector Position = GetOwner()->GetActorLocation();
	
	if (this->bOpen)
	{
		if (Position != this->EndPosition)
		{
			Position.X = FMath::FInterpConstantTo(Position.X, this->EndPosition.X, DeltaTime, this->ScalarMove);
			Position.Y = FMath::FInterpConstantTo(Position.Y, this->EndPosition.Y, DeltaTime, this->ScalarMove);
			Position.Z = FMath::FInterpConstantTo(Position.Z, this->EndPosition.Z, DeltaTime, this->ScalarMove);
		}
	}
	else if (!this->bOpen)
	{
		if (Position != this->BeginPosition)
		{
			Position.X = FMath::FInterpConstantTo(Position.X, this->BeginPosition.X, DeltaTime, this->ScalarMove);
			Position.Y = FMath::FInterpConstantTo(Position.Y, this->BeginPosition.Y, DeltaTime, this->ScalarMove);
			Position.Z = FMath::FInterpConstantTo(Position.Z, this->BeginPosition.Z, DeltaTime, this->ScalarMove);
		}
	}
	
	if (Position == this->EndPosition || Position == this->BeginPosition)
		if (!this->bFinishedAnimation)
			this->bFinishedAnimation = true;
	
	GetOwner()->SetActorLocation(Position);
}