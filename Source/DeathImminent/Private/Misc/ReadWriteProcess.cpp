#include "Misc/ReadWriteProcess.h"
#include "Systems/ServerManagingSubsystem.h"

FReadWriteProcess::FReadWriteProcess(UServerManagingSubsystem* ServerManagingSubsystem,
									 const FString& InURL,
									 const FString& InParams,
									 bool InHidden,
									 bool InCreatePipes) : FMonitoredProcess(InURL, InParams, InHidden, InCreatePipes)
{
	this->ServerManagingSubsystem = ServerManagingSubsystem;
}

FReadWriteProcess::FReadWriteProcess(UServerManagingSubsystem* ServerManagingSubsystem, 
									 const FString& InURL, 
									 const FString& InParams, 
									 const FString& InWorkingDir,
									 bool InHidden, 
									 bool InCreatePipes) : FMonitoredProcess(InURL, InParams, InWorkingDir, InHidden, InCreatePipes)
{
	this->ServerManagingSubsystem = ServerManagingSubsystem;
}

bool FReadWriteProcess::SendMessage(const FString& Message)
{
	return FPlatformProcess::WritePipe(WritePipe, Message);
}

void FReadWriteProcess::OnCompleted(int32 returnCode)
{

}
