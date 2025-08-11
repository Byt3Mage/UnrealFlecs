#include "FlecsInventoryContainer.h"

bool FFlecsInventoryContainer::CanAcceptItem(const FGameplayTag& ItemTag) const
{
	return ItemTag.MatchesAny(AcceptableItems) && !ItemTag.MatchesAny(UnacceptableItems);
}

bool FFlecsInventoryContainer::FindEmptySlotsAtIndex(const int32 Index, const FFlecsInventoryItemSize& ItemSize,
	const UFlecs::HashSet<int32>& Occupied, UFlecs::HashSet<int32>& OutSlots) const
{
	OutSlots.clear();

	const int32 SlotY = Index / NumberOfColumns;
	const int32 SlotX = Index % NumberOfColumns;

	for (int32 Row = SlotY; Row < (SlotY + ItemSize.Height); ++Row)
	{
		for (int32 Col = SlotX; Col < (SlotX + ItemSize.Width); ++Col)
		{
			if (Row >= NumberOfRows || Col >= NumberOfColumns)
			{
				OutSlots.clear();
				return false;
			}
		
			int32 FoundSlot = (Row * NumberOfColumns) + Col;

			if (Occupied.contains(FoundSlot))
			{
				OutSlots.clear();
				return false;
			}
		
			OutSlots.insert(FoundSlot);
		}
	}

	return true;
}

int32 FFlecsInventoryContainer::FindEmptyStack(const FFlecsInventoryItemSize& ItemSize,
	const UFlecs::HashSet<int32>& Occupied, UFlecs::HashSet<int32>& OutSlots, bool& IsRotated) const
{
	const int32 NumSlots = NumberOfRows * NumberOfColumns;

	OutSlots.reserve(ItemSize.Width * ItemSize.Height);

	if (ItemSize.Width <= NumberOfColumns && ItemSize.Height <= NumberOfRows)
	{
		for (int32 i = 0; i < NumSlots; ++i)
		{
			if (FindEmptySlotsAtIndex(i, ItemSize, Occupied, OutSlots))
			{
				IsRotated = false;
				return i;
			}
		}
	}

	//Rotate the item
	if (ItemSize.Width <= NumberOfColumns && ItemSize.Height <= NumberOfRows)
	{
		for (int32 i = 0; i < NumSlots; ++i)
		{
			if (FindEmptySlotsAtIndex(i, ItemSize.GetRotated(), Occupied, OutSlots))
			{
				IsRotated = true;
				return i;
			}
		}
	}

	return INDEX_NONE;
}

bool FFlecsInventoryContainer::AddItem(const flecs::entity& Inventory, const flecs::entity& Item)
{
	const auto* ItemInfo = Item.try_get<FFlecsInventoryItemInfo>();

	if (!ItemInfo || !CanAcceptItem(ItemInfo->Tag))
	{
		return false;
	}

	switch (Type)
	{
	case EFlecsInventoryContainerType::Storage:		  return StoreItem(Inventory, Item, *ItemInfo);
	case EFlecsInventoryContainerType::AttachmentSlot: return EquipItem(Inventory, Item, *ItemInfo);
	default: return false;
	}
}

void FFlecsInventoryContainer::RemoveItem(const flecs::entity& Inventory, const flecs::entity& Item)
{
	const auto& ContainedBy = Item.get<FContainedBy>(Inventory);
	const int32 StackId = ContainedBy.StackId;
	
	if (const auto It = Stacks.find(StackId); It != Stacks.end())
	{
		FFlecsInventoryItemStack& Stack = It->second;
		
		if (Stack.Items.erase(Item) > 0)
		{
			Item.remove<FContainedBy>(Inventory);
		}
		
		if (Stack.Items.empty())
		{
			Stacks.erase(StackId);
		}
	}
}

bool FFlecsInventoryContainer::StoreItem(const flecs::entity& Inventory, const flecs::entity& Item,
	const FFlecsInventoryItemInfo& ItemInfo)
{
	UFlecs::HashSet<int32> Occupied; Occupied.reserve(NumberOfColumns * NumberOfRows);

	for (auto&[StackId, Stack] : Stacks)
	{
		if (ItemInfo.AssetId == Stack.AssetId
			&& (Stack.MaxStackAmount == 0 || Stack.Items.size() < Stack.MaxStackAmount)
			&& Stack.Size.Matches(ItemInfo.Size))
		{
			Stack.Items.insert(Item);
			Item.set(Inventory, FContainedBy{.Container = Name, .StackId = StackId});
			return true;
		}

		Occupied.insert(Stack.OccupiedSlots.begin(), Stack.OccupiedSlots.end());
	}

	UFlecs::HashSet<int32> Slots; bool IsRotated = false;
	
	if (const int32 StackId = FindEmptyStack(ItemInfo.Size, Occupied, Slots, IsRotated); StackId != INDEX_NONE)
	{
		FFlecsInventoryItemStack& Stack = Stacks[StackId];
	
		Stack.AssetId = ItemInfo.AssetId;
		Stack.Size = IsRotated ? ItemInfo.Size.GetRotated() : ItemInfo.Size;
		Stack.MaxStackAmount = FMath::Max(ItemInfo.MaxStackAmount, 0);
		Stack.OccupiedSlots = std::move(Slots);
		Stack.Items.insert(Item);
		
		Item.set(Inventory, FContainedBy{.Container = Name, .StackId = StackId});
		return true;
	}

	return false;
}

bool FFlecsInventoryContainer::EquipItem(const flecs::entity& Inventory, const flecs::entity& Item,
	const FFlecsInventoryItemInfo& ItemInfo)
{
	FFlecsInventoryItemStack& Stack = Stacks[0];

	if (!Stack.Items.empty())
	{
		return false;
	}

	Stack.AssetId = ItemInfo.AssetId;
	Stack.Size = ItemInfo.Size;
	Stack.MaxStackAmount = FMath::Max(ItemInfo.MaxStackAmount, 0);
	Stack.OccupiedSlots = {0};
	Stack.Items.insert(Item);

	Item.set(Inventory, FContainedBy{.Container = Name, .StackId = 0});
	return true;
}

namespace FlecsInventory
{
	bool AddItemToInventory(const flecs::entity Item, const flecs::entity Inventory)
	{
		if (auto* InvComp = Inventory.try_get_mut<FFlecsInventoryComponent>())
		{
			for (auto&[_, Container] : InvComp->Containers)
			{
				if (Container.AddItem(Inventory, Item))
				{
					return true;
				}
			}
		}

		return false;
	}
	
	bool AddItemToInventory(const flecs::entity Item, const flecs::entity Inventory, const FName ContainerName)
	{
		if (auto* InvComp = Inventory.try_get_mut<FFlecsInventoryComponent>())
		{
			if (auto* Container = InvComp->Containers.Find(ContainerName))
			{
				return Container->AddItem(Inventory, Item);
			}
		}

		return false;
	}
}