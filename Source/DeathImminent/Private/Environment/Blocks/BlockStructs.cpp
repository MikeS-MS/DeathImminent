#include "Environment/Blocks/BlockStructs.h"
#include "Environment/WorldGrid/Chunk.h"
#include "Systems/ContentManager.h"
#include "Utilities/GameUtilities.h"

FBaseID FBlock::ToRealID(const AChunk* Chunk) const
{
	if (!IsValid(Chunk))
	{
		CHECK_INSTANCE_RETURN(UContentManager, ContentManager, FBaseID())
		return ContentManager->GetBaseGameContent()->GetAirBlock();
	}
	
	return Chunk->GetRealIDFromLocal(LocalBlockID);
}
