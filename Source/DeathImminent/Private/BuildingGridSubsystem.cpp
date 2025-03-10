// Copyright MikeSMediaStudios™ 2023


#include "BuildingGridSubsystem.h"
#include "Kismet/GameplayStatics.h"


void UBuildingGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	m__OnActorSpawnedDelegate.BindUObject(this, &UBuildingGridSubsystem::__OnActorSpawned);
}

void UBuildingGridSubsystem::Deinitialize()
{
	Super::Deinitialize();
	sm__BuildingGrid = nullptr;
}

void UBuildingGridSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	ABuildingGrid* Grid = Cast<ABuildingGrid>(UGameplayStatics::GetActorOfClass(&InWorld, ABuildingGrid::StaticClass()));

	if (!Grid)
	{
		m__OnActorSpawnedDelegateHandle = InWorld.AddOnActorSpawnedHandler(m__OnActorSpawnedDelegate);
		return;
	}

	sm__BuildingGrid = Grid;
}

void UBuildingGridSubsystem::__OnActorSpawned(AActor* Actor)
{
	ABuildingGrid* Grid = Cast<ABuildingGrid>(Actor);

	if (!Grid)
		return;

	sm__BuildingGrid = Grid;
	GetWorld()->RemoveOnActorSpawnedHandler(m__OnActorSpawnedDelegateHandle);
	m__OnActorSpawnedDelegate.Unbind();
}

ABuildingGrid* UBuildingGridSubsystem::sm__BuildingGrid = nullptr;