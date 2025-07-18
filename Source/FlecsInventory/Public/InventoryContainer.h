#pragma once

#include "flecs.h"
#include "FlecsTypeRegistry.h"
#include "GameplayTagContainer.h"
#include "InventoryItem.h"
#include "UnrealFlecs.h"
#include "InventoryContainer.generated.h"

UENUM(BlueprintType)
enum class EInventoryContainerType : uint8
{
	Storage,
	AttachmentSlot
};

USTRUCT(BlueprintType)
struct FLECSINVENTORY_API FContainerStack
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
struct FLECSINVENTORY_API FInventoryContainer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FName Name = NAME_None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EInventoryContainerType Type = EInventoryContainerType::Storage;
	
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
	
	UnrealFlecs::HashMap<int32, FContainerStack> Stacks = {};

	bool CanAcceptItem(const FGameplayTag& ItemTag) const;
	
	bool FindEmptySlotsAtIndex(const int32 Index, const FFlecsInventoryItemSize& ItemSize,
		const UnrealFlecs::HashSet<int32>& Occupied, UnrealFlecs::HashSet<int32>& OutSlots) const;
	int32 FindEmptyStack(const FFlecsInventoryItemSize& ItemSize, const UnrealFlecs::HashSet<int32>& Occupied,
		UnrealFlecs::HashSet<int32>& OutSlots, bool& IsRotated) const;
	
	bool AddItem   (const flecs::entity& Container, const flecs::entity& Item);
	void RemoveItem(const flecs::entity& Container, const flecs::entity& Item);
	
private:
	bool StoreItem(const flecs::entity& Container, const flecs::entity& Item, const FFlecsInventoryItemInfo& ItemInfo);
	bool EquipItem(const flecs::entity& Container, const flecs::entity& Item, const FFlecsInventoryItemInfo& ItemInfo);
};

REG_FLECS_COMPONENT(FFlecsInventoryComponent)
USTRUCT(BlueprintType)
struct FLECSINVENTORY_API FFlecsInventoryComponent
{
	GENERATED_BODY()
	
	FInventoryContainer* FindContainer(const FName& ContainerName)
	{
		return Containers.Find(ContainerName);
	}

	const FInventoryContainer* FindContainer(const FName& ContainerName) const
	{
		return Containers.Find(ContainerName);
	}
	
private:
	UPROPERTY(EditAnywhere)
	TMap<FName, FInventoryContainer> Containers;
};