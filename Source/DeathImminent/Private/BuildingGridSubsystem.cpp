// Copyright MikeSMediaStudios™ 2023


#include "BuildingGridSubsystem.h"

#include "Environment/BuildingGrid/BuildingGrid.h"
#include "Kismet/GameplayStatics.h"


void UBuildingGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnActorSpawnedDelegate.BindUObject(this, &UBuildingGridSubsystem::OnActorSpawned);
}

void UBuildingGridSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	ABuildingGrid* Grid = Cast<ABuildingGrid>(UGameplayStatics::GetActorOfClass(&InWorld, ABuildingGrid::StaticClass()));

	if (!Grid)
	{
		OnActorSpawnedDelegateHandle = InWorld.AddOnActorSpawnedHandler(OnActorSpawnedDelegate);
		return;
	}

	m_BuildingGrid = Grid;
}

void UBuildingGridSubsystem::OnActorSpawned(AActor* Actor)
{
	ABuildingGrid* Grid = Cast<ABuildingGrid>(Actor);

	if (!Grid)
		return;

	m_BuildingGrid = Grid;
	GetWorld()->RemoveOnActorSpawnedHandler(OnActorSpawnedDelegateHandle);
	OnActorSpawnedDelegate.Unbind();
}

const ABuildingGrid* UBuildingGridSubsystem::GetBuildingGrid() const
{
	return m_BuildingGrid;
}
