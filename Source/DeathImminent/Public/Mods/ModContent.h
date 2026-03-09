#pragma once
#include "CoreMinimal.h"
#include "ModContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogModContent, All, All);

UCLASS(BlueprintType, Blueprintable)
class DEATHIMMINENT_API UModContent : public UObject
{
	friend class UContentManager;

	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName m_Name;
	
	UPROPERTY(Category = "Data", Config, EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	TArray<TSoftObjectPtr<UDataTable>> m_Items;

	UPROPERTY(Category = "Data", Config, EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	TArray<TSoftObjectPtr<UDataTable>> m_Blocks;

	UPROPERTY(Category = "Data", Config, EditAnywhere, BlueprintReadOnly, AdvancedDisplay)
	TArray<TSoftObjectPtr<UDataTable>> m_Structures;
	
private:

	UPROPERTY(VisibleAnywhere)
	FGuid m__Guid;
	
public:

	UFUNCTION(BlueprintCallable)
	const FName& GetModName() const;
	
	UFUNCTION(BlueprintCallable)
	const FGuid& GetGuid() const
	{
		return m__Guid;
	}	
	
	UFUNCTION(BlueprintCallable)
	const TArray<TSoftObjectPtr<UDataTable>>& GetBlocksDataTable() const
	{
		return m_Blocks;
	}	
	
	UFUNCTION(BlueprintCallable)
	const TArray<TSoftObjectPtr<UDataTable>>& GetItemsDataTable() const
	{
		return m_Items;
	}	
	
	UFUNCTION(BlueprintCallable)
	const TArray<TSoftObjectPtr<UDataTable>>& GetStructuresDataTable() const
	{
		return m_Structures;
	}
	
	UFUNCTION(BlueprintCallable)
	const UModContent* GetInstance() const;
	
protected:
	
	UFUNCTION(BlueprintNativeEvent)
	void _Initialize();
	virtual void _Initialize_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	void _Deinitialize();
	virtual void _Deinitialize_Implementation();
	
};
