// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Door.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HORRORESCAPE_API UDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	UFUNCTION(BlueprintCallable)
	void Toggle();
private:
	virtual void Update(float DeltaTime);
protected:
	bool bFinishedAnimation{ true };
	UPROPERTY(EditAnywhere)
	float ScalarMove{ 150.f };
	
	/*Enable testing/debugging*/
	UPROPERTY(EditAnywhere)
	bool Debug{ false };

	/*Set door as open/closed by default*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = "Debug"))
	bool bOpen{ false };
};
