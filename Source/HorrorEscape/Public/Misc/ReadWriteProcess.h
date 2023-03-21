#pragma once

#include "CoreMinimal.h"
#include "Misc/MonitoredProcess.h"
//#include "ReadWriteProcess.generated.h"

class HORRORESCAPE_API FReadWriteProcess : public FMonitoredProcess
{
public:
	FReadWriteProcess(const FString& InURL, const FString& InParams, bool InHidden, bool InCreatePipes = true);
	FReadWriteProcess(const FString& InURL, const FString& InParams, const FString& InWorkingDir, bool InHidden, bool InCreatePipes = true);

	bool SendMessage(const FString& Message);
};