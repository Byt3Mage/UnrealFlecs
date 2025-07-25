#pragma once
#include "FlecsInventoryComponents.h"
#include "FlecsTypeRegistry.h"
#include "GameplayTagContainer.h"
#include "FlecsInventoryContainer.generated.h"

UENUM(BlueprintType)
enum class EFlecsInventoryContainerType : uint8
{
	Storage,
	AttachmentSlot
};

USTRUCT(BlueprintType)
struct FLECSINVENTORY_API FFlecsInventoryItemStack
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FPrimaryAssetId AssetId = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FFlecsInventoryItemSize Size = 1;

	UPROPERTY()
	int32 MaxStackAmount = 1;

	UnrealFlecs::HashSet<int32> OccupiedSlots = {};
	UnrealFlecs::HashSet<flecs::entity> Items = {};
};

USTRUCT(BlueprintType)
struct FLECSINVENTORY_API FFlecsInventoryContainer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName Name = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EFlecsInventoryContainerType Type = EFlecsInventoryContainerType::Storage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 NumberOfRows = 1;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 NumberOfColumns = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer AcceptableItems = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer UnacceptableItems = {};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName AttachmentSocket = NAME_None;
	
	UnrealFlecs::HashMap<int32, FFlecsInventoryItemStack> Stacks = {};

	bool CanAcceptItem(const FGameplayTag& ItemTag) const;
	
	bool FindEmptySlotsAtIndex(const int32 Index, const FFlecsInventoryItemSize& ItemSize,
		const UnrealFlecs::HashSet<int32>& Occupied, UnrealFlecs::HashSet<int32>& OutSlots) const;
		
	int32 FindEmptyStack(const FFlecsInventoryItemSize& ItemSize, const UnrealFlecs::HashSet<int32>& Occupied,
		UnrealFlecs::HashSet<int32>& OutSlots, bool& IsRotated) const;
	
	bool AddItem   (const flecs::entity& Inventory, const flecs::entity& Item);
	void RemoveItem(const flecs::entity& Inventory, const flecs::entity& Item);
	
private:
	bool StoreItem(const flecs::entity& Inventory, const flecs::entity& Item, const FFlecsInventoryItemInfo& ItemInfo);
	bool EquipItem(const flecs::entity& Inventory, const flecs::entity& Item, const FFlecsInventoryItemInfo& ItemInfo);
};

USTRUCT(BlueprintType)
struct FLECSINVENTORY_API FFlecsInventoryComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TMap<FName, FFlecsInventoryContainer> Containers;
};

namespace FlecsInventory
{
	FLECSINVENTORY_API bool AddItemToInventory(flecs::entity Item, flecs::entity Inventory);
	FLECSINVENTORY_API bool AddItemToInventory(flecs::entity Item, flecs::entity Inventory, FName ContainerName);
}