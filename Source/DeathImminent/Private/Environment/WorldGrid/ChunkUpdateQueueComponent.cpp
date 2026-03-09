// Copyright MikeSMediaStudios™

#include "Environment/WorldGrid/ChunkUpdateQueueComponent.h"
#include "Async/Async.h"
#include "Environment/WorldGrid/WorldGrid.h"
#include "Environment/WorldGrid/Chunk.h"
#include "Environment/WorldGrid/GridMeshUtilities.h"

FUpdateChunkThread::FUpdateChunkThread(const int32 ID)
{
	m__Thread = FRunnableThread::Create(this, *FString("ChunkUpdateThread No: " + FString::FromInt(ID)));
}

uint32 FUpdateChunkThread::Run()
{
	while (mb__Running)
	{

		if (mb__Paused)
		{
			FPlatformProcess::Sleep(.25f);
			continue;
		}

		mb__Occupied = true;

		UGridMeshUtilities::MarchingCubes(m__CurrentChunk, m__CurrentChunk->m__CalculatedMeshData, mb__StopEarly);

		FOnCompletedDelegate OnCompleted;
		OnCompleted.BindStatic(&FUpdateChunkThread::NotifyCompleted, m__CurrentChunk, mb__StopEarly);

		AsyncTask(ENamedThreads::GameThread, [OnCompleted]()
		{
			OnCompleted.ExecuteIfBound();
		});

		mb__StopEarly = false;
		mb__Paused = true;
		mb__Occupied = false;
	}
	return 0;
}

void FUpdateChunkThread::Stop()
{
	FRunnable::Stop();

	mb__StopEarly = true;
	mb__Paused = true;
	mb__Running = false;
}

void FUpdateChunkThread::Exit()
{
	FRunnable::Exit();
}

FUpdateChunkThread::~FUpdateChunkThread()
{
	Destroy();
}

void FUpdateChunkThread::Start(AChunk* NewChunk)
{
	m__CurrentChunk = NewChunk;
	mb__Occupied = true;
	mb__StopEarly = false;
	mb__Paused = false;
}

void FUpdateChunkThread::NotifyCompleted(AChunk* Chunk, const bool StoppedEarly)
{
	// UE_LOG(LogTemp, Error, TEXT("Finished Updating Chunk %s"), *Chunk->GetName())
	if (!StoppedEarly)
		if (IsValid(Chunk))
			Chunk->__OnFinishedCalculatingFromThread();

	if (sm__ChunkUpdateQueueComponent)
		sm__ChunkUpdateQueueComponent->__CheckThreads();
}

void FUpdateChunkThread::StopEarly()
{
	m__Thread->Suspend();
	mb__StopEarly = true;
	m__Thread->Suspend(false);
}

bool FUpdateChunkThread::StopEarlyIfWorkingOnChunk(AChunk* Chunk)
{
	m__Thread->Suspend();
	if (IsChunkBeingWorkedOn(Chunk))
	{
		StopEarly();
		m__Thread->Suspend(false);
		return true;
	}
	m__Thread->Suspend(false);
	return false;
}

void FUpdateChunkThread::Destroy()
{
	if (m__Thread)
	{
		m__Thread->Kill();
		delete m__Thread;
	}

	m__Thread = nullptr;
}

bool FUpdateChunkThread::IsChunkBeingWorkedOn(AChunk* Chunk) const
{
	return mb__Occupied && m__CurrentChunk == Chunk;
}

UChunkUpdateQueueComponent* FUpdateChunkThread::sm__ChunkUpdateQueueComponent{ nullptr };

UChunkUpdateQueueComponent::UChunkUpdateQueueComponent()
{
	sm__Instance = this;

}

void UChunkUpdateQueueComponent::BeginPlay()
{
	Super::BeginPlay();

	__SetupThreads();
	__CheckThreads();
}

void UChunkUpdateQueueComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (FUpdateChunkThread* Thread : m__PermanentThreads)
		Thread->StopEarly();

	m__QueuedChunks.Empty();
	for (FUpdateChunkThread* Thread : m__PermanentThreads)
		delete Thread;

	m__PermanentThreads.Empty();
	FUpdateChunkThread::sm__ChunkUpdateQueueComponent = nullptr;
	sm__Instance = nullptr;
}

void UChunkUpdateQueueComponent::__SetupThreads()
{
	m__PermanentThreads.SetNum(AmountOfThreads);

	for (int i = 0; i < m__PermanentThreads.Num(); i++)
	{
		m__PermanentThreads[i] = new FUpdateChunkThread(i);
	}

	FUpdateChunkThread::sm__ChunkUpdateQueueComponent = this;
}

void UChunkUpdateQueueComponent::__EnqueueChunk(AChunk* ChunkToUpdate)
{
	m__QueuedChunks.EnqueueSafe(ChunkToUpdate);
	__CheckThreads();
}

void UChunkUpdateQueueComponent::__CancelUpdateForChunk(AChunk* ChunkToCancel)
{
	for (FUpdateChunkThread* UpdateChunkThread : m__PermanentThreads)
	{
		if (UpdateChunkThread->IsChunkBeingWorkedOn(ChunkToCancel))
		{
			if (UpdateChunkThread->StopEarlyIfWorkingOnChunk(ChunkToCancel))
			{
				m__QueuedChunks.RemoveUnique(ChunkToCancel);

				return;
			}
		}
	}
}

void UChunkUpdateQueueComponent::__CheckThreads()
{
	for (FUpdateChunkThread* UpdateChunkThread : m__PermanentThreads)
	{
		if (UpdateChunkThread->mb__Occupied)
			continue;

		AChunk* Chunk = m__QueuedChunks.PeekSafe();

		if (!Chunk)
			return;

		if (__IsChunkBeingWorkedOn(Chunk))
		{
			m__QueuedChunks.PopUnsafe();
			m__QueuedChunks.EnqueueUnsafe(Chunk);
			continue;
		}

		m__QueuedChunks.PopSafe(Chunk);
		UpdateChunkThread->Start(Chunk);
	}
}

bool UChunkUpdateQueueComponent::__IsChunkBeingWorkedOn(AChunk* Chunk) const
{
	for (const FUpdateChunkThread* UpdateChunkThread : m__PermanentThreads)
	{
		if (UpdateChunkThread->IsChunkBeingWorkedOn(Chunk))
			return true;
	}

	return false;
}

UChunkUpdateQueueComponent* UChunkUpdateQueueComponent::sm__Instance = nullptr;
