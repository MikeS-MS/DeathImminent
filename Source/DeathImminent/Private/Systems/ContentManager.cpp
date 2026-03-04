#include "Systems/ContentManager.h"

void UContentManager::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstanceSubsystem::Initialize(Collection);

	for (TObjectIterator<UClass> It; It; ++It)
	{
		if(It->IsChildOf(UModContent::StaticClass()) && !m_LoadedMods.Contains(*It))
		{
			UModContent* instance = Cast<UModContent>(It->CreateDefaultSubobject(FName(*It->GetName()),
																				 It->GetClass(),
																				 It->GetClass(),
																				 true,
																				 true));
			instance->_Initialize();
			m_LoadedMods.Add(*It, instance);
		}
	}
}

const UModContent* UContentManager::GetModInstance(TSubclassOf<UModContent> modContentClass) const
{
	if (!m_LoadedMods.Contains(modContentClass))
		UE_LOG(LogContentManager, Fatal, TEXT("%s mod not loaded"), *modContentClass->GetName())
	return m_LoadedMods[modContentClass];
}
