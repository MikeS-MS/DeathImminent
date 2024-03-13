#pragma once

#include "CoreMinimal.h"
#include "VoxelDataMeshStructs.h"
#include "Environment/Blocks/BlockStructs.h"
#include "HAL/Runnable.h"
#include "Async/Future.h"
#include "ChunkUpdateQueueComponent.generated.h"

#define NUM_OF_CHUNK_UPDATE_THREADS 4

USTRUCT()
struct FCompletedThreadData
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 Result = 1;

	UPROPERTY()
	FVoxelMeshSectionData MeshData;
};


class AChunk;

class FUpdateChunkThread : public FRunnable
{
	friend class UChunkUpdateQueueComponent;

public:

	DECLARE_EVENT(FUpdateChunkThread, FOnCompletedEvent);

	FOnCompletedEvent OnCompleted;

public:

	FUpdateChunkThread(AChunk* NewChunk);
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	virtual ~FUpdateChunkThread() override;

	void Start();
	void NotifyCompleted();
	FCompletedThreadData GetData();

private:

	TAtomic<bool> mb__Occupied = false;
	FString Name;
	FUpdateMeshInformation m__UpdateMeshInformation;
	FCompletedThreadData m__CompletedThreadData;
	FCriticalSection m__Mutex;
	FUpdateChunkThread* m__OriginalThread;
	FRunnableThread* m__Thread;
	TArray<FBlock> m__Blocks;
};

UCLASS()
class UChunkUpdateQueueComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class AChunk;
	friend class FUpdateChunkThread;

public:

	virtual void BeginPlay() override;

private:

	void __EnqueueChunk(AChunk* ChunkToUpdate);

	void __SetupThreads();
	void __CheckThreads();

	bool __IsThereWork();

private:

	TArray<AChunk*> m__QueuedChunks;
	TArray<FUpdateChunkThread*> m__PermanentThreads;

};