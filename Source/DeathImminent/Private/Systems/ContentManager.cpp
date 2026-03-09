#include "Systems/ContentManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"

void UContentManager::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstanceSubsystem::Initialize(Collection);
	sm__Instance = this;
	__Setup();
}

void UContentManager::Deinitialize()
{
	Super::Deinitialize();

	sm__Instance = nullptr;
}

const UModContent* UContentManager::GetModInstance(TSubclassOf<UModContent> modContentClass) const
{
	if (!m_LoadedMods.Contains(modContentClass))
		UE_LOG(LogContentManager, Fatal, TEXT("%s mod not loaded"), *modContentClass->GetName())
	return m_LoadedMods[modContentClass];
}

void UContentManager::__Setup()
{
	FAssetRegistryModule& AssetRegistryModule =
			FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	IAssetRegistry& Registry = AssetRegistryModule.Get();

	TArray<FAssetData> BlueprintAssets;

	Registry.GetAssetsByClass(
		UBlueprint::StaticClass()->GetClassPathName(),
		BlueprintAssets,
		true
	);

	for (const FAssetData& Asset : BlueprintAssets)
	{
		const FString ParentClassPath =
			Asset.TagsAndValues.FindTag(TEXT("ParentClass")).AsString();

		if (ParentClassPath.IsEmpty()) continue;

		FString ClassPath = FPackageName::ExportTextPathToObjectPath(*ParentClassPath);
		UClass* ParentClass = LoadObject<UClass>(nullptr, *ClassPath);

		if (!ParentClass) continue;

		if (ParentClass->IsChildOf(UModContent::StaticClass()))
		{
			UBlueprint* BP = Cast<UBlueprint>(Asset.GetAsset());

			if (!BP || !BP->GeneratedClass) continue;

			if (BP->GeneratedClass->HasAnyClassFlags(CLASS_Abstract))
				continue;

			UModContent* Instance =
				NewObject<UModContent>(GetGameInstance(), BP->GeneratedClass);
			if (UDeathImminentContent* DeathImminentContentMod = Cast<UDeathImminentContent>(Instance))
				m__BaseGameContent = DeathImminentContentMod;
			m_LoadedMods.Add(BP->GeneratedClass.Get(), Instance);
		}
	}
}

UContentManager* UContentManager::sm__Instance = nullptr;
