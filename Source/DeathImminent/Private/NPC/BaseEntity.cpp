#include "NPC/BaseEntitiy.h"

ABaseEntity::ABaseEntity()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ABaseEntity::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABaseEntity::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABaseEntity::BeginPlay()
{
	Super::BeginPlay();
}
