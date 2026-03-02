#include "NPC/SurvivalPlayer.h"
#include "Items/ItemInventoryComponent.h"



ASurvivalPlayer::ASurvivalPlayer()
{
	m_ItemInventoryComponent = CreateDefaultSubobject<UItemInventoryComponent>(TEXT("ItemInventory"));
}

void ASurvivalPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASurvivalPlayer::BeginPlay()
{
	Super::BeginPlay();
}
