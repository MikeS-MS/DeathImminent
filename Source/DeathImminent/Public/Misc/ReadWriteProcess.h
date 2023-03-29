#pragma once

#include "CoreMinimal.h"
#include "Misc/MonitoredProcess.h"
// #include "ReadWriteProcess.generated.h"

class UServerManagingSubsystem;

class DEATHIMMINENT_API FReadWriteProcess : public FMonitoredProcess
{
private:
	UServerManagingSubsystem *ServerManagingSubsystem;

public:
	FReadWriteProcess(UServerManagingSubsystem *ServerManagingSubsystem, const FString &InURL, const FString &InParams, bool InHidden, bool InCreatePipes = true);
	FReadWriteProcess(UServerManagingSubsystem *ServerManagingSubsystem, const FString &InURL, const FString &InParams, const FString &InWorkingDir, bool InHidden, bool InCreatePipes = true);

	bool SendMessage(const FString &Message);

private:
	void OnCompleted(int32 returnCode);
};