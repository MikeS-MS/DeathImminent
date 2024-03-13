// Copyright MikeSMediaStudios™ 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TestInventoryComponent.generated.h"

class UItemInventoryComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHIMMINENT_API UTestInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTestInventoryComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;
	void OpenInventory(UItemInventoryComponent* OwningInventory);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
