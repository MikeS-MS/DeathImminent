// Copyright MikeSMediaStudios™

#pragma once

#include "CoreMinimal.h"
#include "GridStructs.h"
#include "Components/ActorComponent.h"
#include "Utilities/GeneralStructs.h"
#include "GridModifyingComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEATHIMMINENT_API UGridModifyingComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBlockAddedToGridDelegate, const FBaseID&, BlockID, const int32, OriginalAmount, const int32, AmountLeft);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBlockRemovedFromGridDelegate, const FBaseID&, BlockID, const int32, Amount);

public:

	UGridModifyingComponent();
	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	void BreakFromGridTest(const FVector& Location);

	UFUNCTION(BlueprintCallable)
	void AddToGridTest(const FVector& Location, const int32 Amount);

	UFUNCTION(BlueprintCallable)
	void AddToGridMultipleTest(const FVector& Location, const FMultipleBlockEditing& MultipleBlocks);

protected:

	UPROPERTY(DisplayName = "OnBlockAddedToGrid", Category = "Delegates", BlueprintCallable)
	FOnBlockAddedToGridDelegate m_OnBlockAddedToGrid;

	UPROPERTY(DisplayName = "OnBlockRemovedFromGridDelegate", Category = "Delegates", BlueprintCallable)
	FOnBlockRemovedFromGridDelegate m_OnBlockRemovedFromGridDelegate;

protected:

	UFUNCTION(DisplayName = "AddBlockToGrid", Category = "Operations", Server, Unreliable, meta = (BlueprintProtected))
	void _AddBlockToGrid(const FVector& Location, const FBaseID& BlockID, const int32 Amount);
	void _AddBlockToGrid_Implementation(const FVector& Location, const FBaseID& BlockID, const int32 Amount);

	UFUNCTION(DisplayName = "RemoveBlockFromGrid", Category = "Operations", Server, Unreliable, meta = (BlueprintProtected))
	void _RemoveBlockFromGrid(const FVector& Location, const int32 Amount);
	void _RemoveBlockFromGrid_Implementation(const FVector& Location, const int32 Amount);
};