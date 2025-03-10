// Copyright MikeSMediaStudios™ 2021


#include "Systems/ServerManagingSubsystem.h"


void UServerManagingSubsystem::StartInformationServer()
{
	if (InformationServerProcess)
		return;

	InformationServerProcess = new FReadWriteProcess(this, InformationServerPath(), TEXT(""), true);
	InformationServerProcess->OnOutput().BindUObject(this, &UServerManagingSubsystem::ParseInformationServer);
	InformationServerProcess->Launch();
}

void UServerManagingSubsystem::StartServer(FString MapName)
{
	
}

void UServerManagingSubsystem::CloseAllServers()
{
	InformationServerProcess->Cancel();
	delete InformationServerProcess;

	//TODO: Close all Dedicated Servers
}

void UServerManagingSubsystem::ParseInformationServer(FString Input)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *Input);
}