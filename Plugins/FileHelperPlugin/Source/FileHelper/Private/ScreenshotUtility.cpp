// Copyright 2022 RLoris

#include "ScreenshotUtility.h"
#include "ImageUtils.h"
#include "UnrealClient.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"

UScreenshotUtility* UScreenshotUtility::TakeScreenshot(UObject* WorldContextObject, FString Filename, bool PrefixTimestamp, bool ShowUI)
{
	UScreenshotUtility* Node = NewObject<UScreenshotUtility>();
	Node->Active = false;
	Node->WorldContextObject = WorldContextObject;
	Node->ShowUI = ShowUI;
	Node->Filename = FPaths::GetCleanFilename(Filename);
	Node->PrefixTimestamp = PrefixTimestamp;
	// not garbage collected
	Node->AddToRoot();
	return Node;
}

void UScreenshotUtility::Activate()
{
	if (nullptr == this->WorldContextObject)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid WorldContextObject. Cannot execute ScreenshotUtility"), ELogVerbosity::Error);
		_Failed();
		return;
	}
	if (this->Active)
	{
		FFrame::KismetExecutionMessage(TEXT("ScreenshotUtility is already running"), ELogVerbosity::Warning);
		_Failed();
		return;
	}
	FText ErrorFilename;
	Filename = FPaths::GetCleanFilename(Filename);
	if (!FFileHelper::IsFilenameValidForSaving(Filename, ErrorFilename))
	{
		FFrame::KismetExecutionMessage(TEXT("Filename is not valid"), ELogVerbosity::Warning);
		_Failed();
		return;
	}
	const FString FinalFilename = (this->PrefixTimestamp ? (FDateTime::Now().ToString(TEXT("%Y_%m_%d__%H_%M_%S__"))) : "") + this->Filename;
	if (!FScreenshotRequest::IsScreenshotRequested())
	{
		this->Active = true;
		ScreenshotTexture = nullptr;
		if (ShowUI)
		{
			FScreenshotRequest::Reset();
			FScreenshotRequest::RequestScreenshot(FinalFilename, this->ShowUI, false);
			FScreenshotRequest::OnScreenshotRequestProcessed().RemoveAll(this);
			FScreenshotRequest::OnScreenshotRequestProcessed().AddUObject(this, &UScreenshotUtility::_Completed, FScreenshotRequest::GetFilename());
		}
		else
		{
			CreateScreenshotWithoutUI(FinalFilename);
		}
	}
}

void UScreenshotUtility::_Completed(const FString Path)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	if (!ScreenshotTexture && FileManager.FileExists(*Path)) 
	{
		ScreenshotTexture = FImageUtils::ImportFileAsTexture2D(Path);
	}
	if (ScreenshotTexture)
	{
		Completed.Broadcast(ScreenshotTexture, Path);
	}
	else
	{
		ScreenshotTexture = nullptr;
		Failed.Broadcast(ScreenshotTexture, TEXT(""));
	}
	this->Active = false;
	FScreenshotRequest::OnScreenshotRequestProcessed().RemoveAll(this);
	RemoveFromRoot();
}

void UScreenshotUtility::_Failed()
{
	ScreenshotTexture = nullptr;
	Failed.Broadcast(ScreenshotTexture, TEXT(""));
	this->Active = false;
	FScreenshotRequest::OnScreenshotRequestProcessed().RemoveAll(this);
	RemoveFromRoot();
}

void UScreenshotUtility::CreateScreenshotWithoutUI(const FString& FinalName)
{
	const UWorld* World = WorldContextObject->GetWorld();
	if (!World || !World->GetGameViewport() || !World->GetGameViewport()->Viewport)
	{
		_Failed();
		return;
	}
	const APlayerCameraManager * PlayerCamera = UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0);
	if (!PlayerCamera)
	{
		_Failed();
		return;
	}
	const FViewport* GameViewport = World->GetGameViewport()->Viewport;
	const FIntRect ViewRect(0, 0, GameViewport->GetSizeXY().X, GameViewport->GetSizeXY().Y);
	USceneCaptureComponent2D* SceneComponent = NewObject<USceneCaptureComponent2D>(this, TEXT("SceneComponent"));
	SceneComponent->RegisterComponentWithWorld(WorldContextObject->GetWorld());
	SceneComponent->bCaptureEveryFrame = false;
	SceneComponent->bCaptureOnMovement = false;
	SceneComponent->bAlwaysPersistRenderingState = true;
	SceneComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
	SceneComponent->FOVAngle = PlayerCamera->GetFOVAngle();
	const FVector CameraLocation = PlayerCamera->GetCameraLocation();
	const FRotator CameraRotation = PlayerCamera->GetCameraRotation();
	SceneComponent->SetWorldLocationAndRotation(CameraLocation, CameraRotation);
	UTextureRenderTarget2D * TextureRenderTarget = NewObject<UTextureRenderTarget2D>();
	TextureRenderTarget->InitCustomFormat(ViewRect.Width(),ViewRect.Height(),PF_B8G8R8A8,false);
	TextureRenderTarget->UpdateResourceImmediate();
	SceneComponent->TextureTarget = TextureRenderTarget;
	SceneComponent->CaptureScene();
	TArray<FColor> OutColors;
	OutColors.Reserve(ViewRect.Width() * ViewRect.Height());
	TextureRenderTarget->GameThread_GetRenderTargetResource()->ReadPixels(OutColors);
	OutColors.Shrink();
	SceneComponent->UnregisterComponent();
	if (OutColors.Num() == 0)
	{
		_Failed();
		return;
	}
	const FString ScreenshotPath = FPaths::ScreenShotDir() + FinalName;
	TArray<uint8> OutImage;
	FImageUtils::ThumbnailCompressImageArray(ViewRect.Width(), ViewRect.Height(), OutColors, OutImage);
	if (OutImage.Num() == 0)
	{
		_Failed();
		return;
	}
	if(!FFileHelper::SaveArrayToFile(OutImage, *ScreenshotPath))
	{
		_Failed();
		return;
	}
	ScreenshotTexture = FImageUtils::ImportBufferAsTexture2D(OutImage);
	_Completed(ScreenshotPath);
}
