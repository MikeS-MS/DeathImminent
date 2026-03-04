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
	
private:

	UPROPERTY(VisibleAnywhere)
	FGuid m__Guid;
	
public:

	UFUNCTION(BlueprintCallable)
	const FName& GetModName() const;
	
	UFUNCTION(BlueprintCallable)
	const FGuid& GetGuid() const;
	
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
