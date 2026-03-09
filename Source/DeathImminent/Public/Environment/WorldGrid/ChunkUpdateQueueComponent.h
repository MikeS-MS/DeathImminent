// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Utilities/GameUtilities.h"
#include "ChunkUpdateQueueComponent.generated.h"

#define NUM_OF_CHUNK_UPDATE_THREADS 4

class AChunk;
class UChunkUpdateQueueComponent;

class FUpdateChunkThread : public FRunnable
{
	friend class UChunkUpdateQueueComponent;

public:

	DECLARE_DELEGATE(FOnCompletedDelegate);

public:

	FUpdateChunkThread(const int32 ID);
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	virtual ~FUpdateChunkThread() override;

	void Start(AChunk* NewChunk);
	static void NotifyCompleted(AChunk* Chunk, const bool StoppedEarly);
	void StopEarly();
	bool StopEarlyIfWorkingOnChunk(AChunk* Chunk);
	void Destroy();

	bool IsChunkBeingWorkedOn(AChunk* Chunk) const;

private:

	bool mb__Occupied{ false };
	bool mb__Running{ true };
	bool mb__StopEarly{ false };
	bool mb__Paused{ true };

	FRunnableThread* m__Thread = nullptr;
	AChunk* m__CurrentChunk = nullptr;
	static UChunkUpdateQueueComponent* sm__ChunkUpdateQueueComponent;
};

UCLASS()
class DEATHIMMINENT_API UChunkUpdateQueueComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class AChunk;
	friend class FUpdateChunkThread;

public:

	UPROPERTY(EditAnywhere)
	int32 AmountOfThreads = NUM_OF_CHUNK_UPDATE_THREADS;

public:

	UChunkUpdateQueueComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	static UChunkUpdateQueueComponent* GetInstance()
	{
		return sm__Instance;
	}

private:

	void __SetupThreads();
	void __EnqueueChunk(AChunk* ChunkToUpdate);
	void __CancelUpdateForChunk(AChunk* ChunkToCancel);
	void __CheckThreads();

	bool __IsChunkBeingWorkedOn(AChunk* Chunk) const;

private:
	
	TCheckedQueue<AChunk*> m__QueuedChunks;
	TArray<FUpdateChunkThread*> m__PermanentThreads;
	static UChunkUpdateQueueComponent* sm__Instance;

};