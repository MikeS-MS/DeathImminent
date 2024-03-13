#include "Environment/BuildingGrid/ChunkUpdateQueueComponent.h"
#include "Environment/BuildingGrid/BuildingGrid.h"
#include "Environment/BuildingGrid/GridData.h"
#include "Environment/BuildingGrid/Chunk.h"
#include "Environment/BuildingGrid/GridMeshUtilities.h"

FUpdateChunkThread::~FUpdateChunkThread()
{
	if (m__Thread)
	{
		m__Thread->Kill();
		delete m__Thread;
	}

	OnCompleted.Clear();
	m__Thread = nullptr;
}

void FUpdateChunkThread::Start()
{
	m__Thread = FRunnableThread::Create(this, *Name);
}

void FUpdateChunkThread::NotifyCompleted()
{
	OnCompleted.Broadcast();
}

FCompletedThreadData FUpdateChunkThread::GetData()
{
	m__Mutex.Lock();
	FCompletedThreadData CompletedThreadData = m__CompletedThreadData;
	m__Mutex.Unlock();

	return CompletedThreadData;
}

FUpdateChunkThread::FUpdateChunkThread(AChunk* NewChunk)
{
	mb__Occupied = true;
	Name = FString("Chunk_" + FString::FromInt(NewChunk->m__ChunkLocationData.IndexInGrid));
	const FGridData GridData = NewChunk->GetContainingGrid()->GetGridData();
	m__UpdateMeshInformation.ChunkSize = GridData.ChunkSizeInBlocks;
	m__UpdateMeshInformation.BlockSize = GridData.BlockSize;
	m__UpdateMeshInformation.UnscaledBoxExtent = NewChunk->m__OverlapCollision->GetUnscaledBoxExtent();
	m__Blocks = NewChunk->__CopyRelevantBlocksForUpdate();
	m__OriginalThread = this;
}

uint32 FUpdateChunkThread::Run()
{
	FCompletedThreadData CompletedThreadData;
	//UGridMeshUtilities::MarchingCubes(m__Blocks, m__UpdateMeshInformation, CompletedThreadData.MeshData);
	CompletedThreadData.Result = 0;

	m__Mutex.Lock();
	m__CompletedThreadData = CompletedThreadData;
	m__Mutex.Unlock();

	return 0;
}

void FUpdateChunkThread::Stop()
{
	FRunnable::Stop();
}

void FUpdateChunkThread::Exit()
{
	mb__Occupied = false;

	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		OnCompleted.Broadcast();
	});

	FRunnable::Exit();
}

void UChunkUpdateQueueComponent::BeginPlay()
{
	Super::BeginPlay();

	__SetupThreads();
}

void UChunkUpdateQueueComponent::__EnqueueChunk(AChunk* ChunkToUpdate)
{
	m__QueuedChunks.AddUnique(ChunkToUpdate);
	__CheckThreads();
}

void UChunkUpdateQueueComponent::__SetupThreads()
{
	m__PermanentThreads.SetNum(NUM_OF_CHUNK_UPDATE_THREADS);

	for (int i = 0; i < m__PermanentThreads.Num(); i++)
	{
		m__PermanentThreads[i] = nullptr;
	}
}

void UChunkUpdateQueueComponent::__CheckThreads()
{
	for (int i = 0; i < m__PermanentThreads.Num(); i++)
	{
		FUpdateChunkThread* UpdateChunkThread = m__PermanentThreads[i];
		if (UpdateChunkThread)
		{
			if (UpdateChunkThread->mb__Occupied)
				continue;
			
			delete m__PermanentThreads[i];
			m__PermanentThreads[i] = nullptr;
		}

		if (!__IsThereWork())
			return;

		AChunk* Chunk = m__QueuedChunks.Pop();
		UpdateChunkThread = new FUpdateChunkThread(Chunk);
		m__PermanentThreads[i] = UpdateChunkThread;
		UpdateChunkThread->OnCompleted.AddLambda([this, Chunk, UpdateChunkThread]()
		{
			const FCompletedThreadData CompletedThreadData = UpdateChunkThread->GetData();
			if (CompletedThreadData.Result == 0)
				Chunk->_UpdateMesh(CompletedThreadData.MeshData);
			__CheckThreads();
		});
		UpdateChunkThread->Start();
	}
}

bool UChunkUpdateQueueComponent::__IsThereWork()
{
	return !m__QueuedChunks.IsEmpty();
}
