// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"

// Sets default values for this component's properties
UDoor::UDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDoor::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	this->Update(DeltaTime);
}

void UDoor::Toggle()
{
	if (this->bFinishedAnimation)
	{
		this->bOpen = !this->bOpen;
		this->bFinishedAnimation = false;
	}
}

void UDoor::Update(float DeltaTime)
{
	
}