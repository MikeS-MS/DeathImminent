#include "Misc/ReadWriteProcess.h"

FReadWriteProcess::FReadWriteProcess(const FString& InURL, const FString& InParams, bool InHidden, bool InCreatePipes) : FMonitoredProcess(InURL, InParams, InHidden, InCreatePipes) {}

FReadWriteProcess::FReadWriteProcess(const FString& InURL, const FString& InParams, const FString& InWorkingDir,
	bool InHidden, bool InCreatePipes) : FMonitoredProcess(InURL, InParams, InWorkingDir, InHidden, InCreatePipes) {}

bool FReadWriteProcess::SendMessage(const FString& Message)
{
	return FPlatformProcess::WritePipe(WritePipe, Message);
}
