#pragma once

#include "Mods/ModContent.h"
#include "Utilities/GeneralStructs.h"
#include "DeathImminentContent.generated.h"

UCLASS(NotBlueprintType, Blueprintable)
class UDeathImminentContent : public UModContent
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(DisplayName = "AirBlock", EditAnywhere, BlueprintReadOnly)
	FBaseID m_AirBlock;	
	
	UPROPERTY(DisplayName = "InvalidBlock", EditAnywhere, BlueprintReadOnly)
	FBaseID m_InvalidBlock;
	
	UPROPERTY(DisplayName = "EmptyItemID", EditAnywhere, BlueprintReadOnly)
	FBaseID m_EmptyItemID;	
	
	UPROPERTY(DisplayName = "InvalidItemID", EditAnywhere, BlueprintReadOnly)
	FBaseID m_InvalidItemID;
	
public:
	
	UFUNCTION(BlueprintCallable)
	const FBaseID& GetAirBlock() const
	{
		return m_AirBlock;
	}	
	
	UFUNCTION(BlueprintCallable)
	const FBaseID& GetInvalidBlock() const
	{
		return m_InvalidBlock;
	}
	
	UFUNCTION(BlueprintCallable)
	const FBaseID& GetEmptyItemID() const
	{
		return m_EmptyItemID;
	}
	
	UFUNCTION(BlueprintCallable)
	const FBaseID& GetInvalidItemID() const
	{
		return m_InvalidItemID;
	}
};
