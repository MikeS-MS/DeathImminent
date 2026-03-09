#pragma once

#include "CoreMinimal.h"
#include "Mods/ModContent.h"
#include "Mods/DeathImminentContent.h"
#include "ContentManager.generated.h"

DECLARE_LOG_CATEGORY_CLASS(LogContentManager, All, All)

UCLASS()
class DEATHIMMINENT_API UContentManager : public UGameInstanceSubsystem 
{
	GENERATED_BODY()

public:
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	const UModContent* GetModInstance(TSubclassOf<UModContent> modContentClass) const;
	
	UFUNCTION(BlueprintCallable)
	const UDeathImminentContent* GetBaseGameContent() const
	{
		return m__BaseGameContent;
	}
	
	UFUNCTION(BlueprintCallable)
	const TMap<TSubclassOf<UModContent>, UModContent*>& GetLoadedMods() const { return m_LoadedMods; }
	
	/**
	 * @brief Be careful calling this as it may return a nullptr.
	 * @warning For C++ use only!
	 */
	static UContentManager* GetInstance()
	{
		return sm__Instance;
	}
	
private:
	
	void __Setup();
	
private:
	
	static UContentManager* sm__Instance;
	
	UDeathImminentContent* m__BaseGameContent;
	TMap<TSubclassOf<UModContent>, UModContent*> m_LoadedMods;
	
};
