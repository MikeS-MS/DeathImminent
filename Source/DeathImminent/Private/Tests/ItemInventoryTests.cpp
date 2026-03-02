#include "ItemInventoryTests.h"
#include "TestGameInstance.h"
#include "Tests/AutomationEditorCommon.h"
#include "Systems/ItemManager.h"
#include "Items/ItemInventoryComponent.h"


bool FInventoryAddDropMoveItemTest::RunTest(const FString& Parameters)
{
	if (!GEditor)
		return false;
	
	UWorld* World = GEditor->GetEditorWorldContext().World();

	UTestGameInstance* GameInstance = NewObject<UTestGameInstance>();

	UItemManager* ItemManager = NewObject<UItemManager>(GameInstance);
	ItemManager->_Setup();
	ItemManager->sm__Instance = ItemManager;

	AActor* InventoryActor = World->SpawnActor(AActor::StaticClass());
	AActor* InventoryActor2 = World->SpawnActor(AActor::StaticClass());
	UItemInventoryComponent* ItemInventory = Cast<UItemInventoryComponent>(InventoryActor->AddComponentByClass(UItemInventoryComponent::StaticClass(), false, FTransform::Identity, false));
	UItemInventoryComponent* ItemInventory2 = Cast<UItemInventoryComponent>(InventoryActor2->AddComponentByClass(UItemInventoryComponent::StaticClass(), false, FTransform::Identity, false));
	const FBaseID ItemID(1);
	ItemInventory->mb_IsPlayerInventory = true;
	ItemInventory->__SetTargetInventory(ItemInventory2);

	// Add Items Test
	ItemInventory->__AddItem(ItemID, 100);
	ItemInventory2->__AddItem(ItemID, 30);

	bool Result = true;
	
	for (int i = 0; i < 5; i++)
	{
		if (!TestNotNull(TEXT("Item No: " + FString::FromInt(i) + " should be filled"), ItemInventory->m__Items[i]))
		{
			Result = false;
			break;
		}

		if (!TestEqual(TEXT("Item No: " + FString::FromInt(i) + " should be ItemID: " + ItemID.ToString()), ItemInventory->m__Items[i]->GetItemID(), ItemID))
		{
			Result = false;
			break;
		}

		if (!TestEqual(TEXT("Item No: " + FString::FromInt(i) + " should be at current amount 20"), ItemInventory->m__Items[i]->GetCurrentAmount(), 20))
		{
			Result = false;
			break;
		}
	}

	// Drop Items Test
	ItemInventory->DropItemOnTopOfItem(ItemInventory->CreateSnapshotForItemAt(0), ItemInventory->CreateSnapshotForItemAt(5));

	if (Result)
		Result = TestNull(TEXT("Item No: " + FString::FromInt(0) + " should not be filled"), ItemInventory->m__Items[0]);

	if (Result)
		Result = TestNotNull(TEXT("Item No: " + FString::FromInt(5) + " should be filled"), ItemInventory->m__Items[5]);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(5) + " should be ItemID: " + ItemID.ToString()), ItemInventory->m__Items[5]->GetItemID(), ItemID);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(5) + " should be at current amount 20"), ItemInventory->m__Items[5]->GetCurrentAmount(), 20);

	ItemInventory->DropItemOnTopOfItem(ItemInventory2->CreateSnapshotForItemAt(0), ItemInventory->CreateSnapshotForItemAt(0));

	if (Result)
		Result = TestNull(TEXT("Item No: " + FString::FromInt(0) + " should not be filled"), ItemInventory2->m__Items[0]);

	if (Result)
		Result = TestNotNull(TEXT("Item No: " + FString::FromInt(0) + " should be filled"), ItemInventory->m__Items[0]);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(0) + " should be ItemID: " + ItemID.ToString()), ItemInventory->m__Items[0]->GetItemID(), ItemID);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(0) + " should be at current amount 20"), ItemInventory->m__Items[0]->GetCurrentAmount(), 20);


	//Move Items Test
	ItemInventory->MoveItemToInventory(ItemInventory2->CreateSnapshotForItemAt(1), -1, ItemInventory);

	if (Result)
		Result = TestNull(TEXT("Item No: " + FString::FromInt(1) + " should not be filled"), ItemInventory2->m__Items[1]);

	if (Result)
		Result = TestNotNull(TEXT("Item No: " + FString::FromInt(6) + " should be filled"), ItemInventory->m__Items[6]);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(6) + " should be ItemID: " + ItemID.ToString()), ItemInventory->m__Items[6]->GetItemID(), ItemID);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(6) + " should be at current amount 10"), ItemInventory->m__Items[6]->GetCurrentAmount(), 10);

	ItemInventory->MoveItemToInventory(ItemInventory->CreateSnapshotForItemAt(6), 5, ItemInventory2);

	if (Result)
		Result = TestNotNull(TEXT("Item No: " + FString::FromInt(0) + " should be filled"), ItemInventory2->m__Items[0]);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(0) + " should be ItemID: " + ItemID.ToString()), ItemInventory2->m__Items[0]->GetItemID(), ItemID);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(0) + " should be at current amount 10"), ItemInventory2->m__Items[0]->GetCurrentAmount(), 5);

	if (Result)
		Result = TestNotNull(TEXT("Item No: " + FString::FromInt(6) + " should be filled"), ItemInventory->m__Items[6]);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(6) + " should be ItemID: " + ItemID.ToString()), ItemInventory->m__Items[6]->GetItemID(), ItemID);

	if (Result)
		Result = TestEqual(TEXT("Item No: " + FString::FromInt(6) + " should be at current amount 10"), ItemInventory->m__Items[6]->GetCurrentAmount(), 5);

	// Cleanup
	for (int i = 0; i < 7; i++)
	{
		if (IsValid(ItemInventory->m__Items[i]))
			ItemInventory->m__Items[i]->Destroy();

		if (i < 2)
		{
			if (IsValid(ItemInventory2->m__Items[i]))
				ItemInventory2->m__Items[i]->Destroy();
		}
	}

	InventoryActor->Destroy();
	InventoryActor2->Destroy();
	ItemInventory = nullptr;
	ItemInventory2 = nullptr;
	InventoryActor = nullptr;
	InventoryActor2 = nullptr;
	World = nullptr;
	ItemManager = nullptr;
	GameInstance = nullptr;
	GEngine->PerformGarbageCollectionAndCleanupActors();
	return Result;
}
