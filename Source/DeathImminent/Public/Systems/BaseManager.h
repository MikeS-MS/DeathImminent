// Copyright MikeSMediaStudios™

#pragma once


#include "Subsystems/GameInstanceSubsystem.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Utilities/GeneralStructs.h"
#include "BaseManager.generated.h"


USTRUCT(BlueprintType)
struct FBaseDataInformation
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Deprecated = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool Redirect = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FBaseID RedirectTo;
};


template<typename T>
using DataMap = TMap<FGuid, TMap<int32, T>>;


UCLASS(BlueprintType)
class DEATHIMMINENT_API UBaseManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	template<typename T>
	static bool __LoadData(DataMap<T>& DataMap, const UDataTable* const DataTable, const FGuid& Source)
	{
		if (!DataTable)
			return false;

		TArray<FName> Rows = DataTable->GetRowNames();

		if (Rows.IsEmpty())
			return false;

		if (!DataMap.Contains(Source))
			DataMap.Emplace(Source, TMap<int32, T>());

		for (const FName& Row : Rows)
		{
			const FString ConvertedRowName = Row.ToString();
			const int32 ID = FCString::Atoi(*ConvertedRowName);
			const T* AbilityInfo = DataTable->FindRow<T>(Row, TEXT("Fetching ability with id" + ConvertedRowName));
			
			DataMap[Source].Emplace(ID, *AbilityInfo);
		}

		return true;
	}

	/**
	 * @brief Checks recursively if an item is valid for the given ID.\n
	 * If it's not loaded it will return false.\n
	 * If it hasn't been redirected it will return true.\n
	 * If it has been deprecated and not redirected it will be false.\n
	 * If it has been deprecated and redirected it will be determined by recursively checking if it has been redirected again, so it can be false if the last item at the chain is deprecated.
	 * Warning! Be careful of creating circular redirections, this will freeze the game.
	 */
	template<typename T>
	static bool __ResolveData(DataMap<T>& DataMap, const FBaseID& ID, T** Data)
	{
		if (__DoesDataExist_Internal(DataMap, ID, Data))
		{
			if ((*Data)->BaseDataInformation.Redirect)
				return __ResolveData(DataMap, (*Data)->BaseDataInformation.RedirectTo, Data);

			return true;
		}

		if (!Data)
			return false;

		if (!(*Data))
			return false;

		if ((*Data)->BaseDataInformation.Redirect)
			return __ResolveData(DataMap, (*Data)->BaseDataInformation.RedirectTo, Data);

		return false;
	}


	template<typename T>
	static bool __DoesDataExist_Internal(DataMap<T>& DataMap, const FBaseID& ID, T** OutData)
	{
		const FGuid& guid = ID.Source->GetDefaultObject<UModContent>()->GetInstance()->GetGuid();
		if (!DataMap.Contains(guid))
			return false;

		if (!DataMap[guid].Contains(ID.ID))
			return false;

		*OutData = &DataMap[guid][ID.ID];

		if ((*OutData)->BaseDataInformation.Deprecated)
			return false;

		return true;
	}
};