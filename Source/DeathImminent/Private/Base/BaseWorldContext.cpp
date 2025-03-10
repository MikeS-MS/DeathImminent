// Copyright MikeSMediaStudios™ 2021


#include "BaseWorldContext.h"

UWorld* UBaseWorldContext::GetWorld() const
{
    if (!GetOuter())
        return nullptr;

    // Special case for behavior tree nodes in the editor
    if (Cast<UPackage>(GetOuter()) != nullptr)
    {
        // GetOuter should return a UPackage and its Outer is a UWorld
        return Cast<UWorld>(GetOuter()->GetOuter());
    }

    // In all other cases...
    return GetOuter()->GetWorld();
}