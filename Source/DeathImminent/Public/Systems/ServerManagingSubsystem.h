// Copyright MikeSMediaStudios™ 2021

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Misc/ReadWriteProcess.h"
#define InformationServerPath() (FPaths::ProjectDir() + /*"Binaries/Win64/"*/ "Test")

/*InformationServer"*/
#include "ServerManagingSubsystem.generated.h"

struct FServerInfo
{

	FMonitoredProcess *ServerProcess;
};

UCLASS()
class DEATHIMMINENT_API UServerManagingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	TArray<FMonitoredProcess *> Servers;
	FReadWriteProcess *InformationServerProcess;

public:
	UFUNCTION(BlueprintCallable)
	void StartInformationServer();
	UFUNCTION(BlueprintCallable)
	void StartServer(FString MapName);
	UFUNCTION(BlueprintCallable)
	void CloseAllServers();

private:
	void ParseInformationServer(FString Input);
};
