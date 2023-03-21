// Copyright 2022 RLoris

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "ScreenshotUtility.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOutputPin, UTexture2D*, Screenshot, FString, Path);

UCLASS()
class FILEHELPER_API UScreenshotUtility : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOutputPin Completed;
	UPROPERTY(BlueprintAssignable)
	FOutputPin Failed;
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Keywords = "File plugin screenshot save load texture", ToolTip = "Take a screenshot, save and load it"), Category = "Screenshot")
	static UScreenshotUtility* TakeScreenshot(UObject* WorldContextObject, FString Filename, bool PrefixTimestamp = true, bool ShowUI = false);
	virtual void Activate() override;
private:
	UFUNCTION()
	void _Completed(const FString Path);
	void _Failed();
	void CreateScreenshotWithoutUI(const FString& FinalName);
private:
	const UObject* WorldContextObject = nullptr;
	bool Active = false;
	FString Filename;
	bool PrefixTimestamp = true;
	bool ShowUI = false;
	UPROPERTY()
	UTexture2D* ScreenshotTexture;
};
