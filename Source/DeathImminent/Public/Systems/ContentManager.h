#pragma once
#include "CoreMinimal.h"
#include "Mods/ModContent.h"
#include "ContentManager.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogContentManager, All, All)

UCLASS(Blueprintable)
class DEATHIMMINENT_API UContentManager : public UGameInstanceSubsystem 
{
	GENERATED_BODY()
	
private:
	
	TMap<TSubclassOf<UModContent>, UModContent*> m_LoadedMods;
	
	
public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	const UModContent* GetModInstance(TSubclassOf<UModContent> modContentClass) const;
};
