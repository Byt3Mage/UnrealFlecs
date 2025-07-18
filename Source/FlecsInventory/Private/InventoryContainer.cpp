#include "InventoryContainer.h"

bool FInventoryContainer::CanAcceptItem(const FGameplayTag& ItemTag) const
{
	return ItemTag.MatchesAny(AcceptableItems) && !ItemTag.MatchesAny(UnacceptableItems);
}

bool FInventoryContainer::FindEmptySlotsAtIndex(const int32 Index, const FFlecsInventoryItemSize& ItemSize,
	const UnrealFlecs::HashSet<int32>& Occupied, UnrealFlecs::HashSet<int32>& OutSlots) const
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

int32 FInventoryContainer::FindEmptyStack(const FFlecsInventoryItemSize& ItemSize,
	const UnrealFlecs::HashSet<int32>& Occupied, UnrealFlecs::HashSet<int32>& OutSlots, bool& IsRotated) const
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

bool FInventoryContainer::AddItem(const flecs::entity& Container, const flecs::entity& Item)
{
	const auto* ItemInfo = Item.try_get<FFlecsInventoryItemInfo>();

	if (!ItemInfo || !CanAcceptItem(ItemInfo->Tag))
	{
		return false;
	}

	switch (Type)
	{
	case EInventoryContainerType::Storage:		  return StoreItem(Container, Item, *ItemInfo);
	case EInventoryContainerType::AttachmentSlot: return EquipItem(Container, Item, *ItemInfo);
	default: return false;
	}
}

void FInventoryContainer::RemoveItem(const flecs::entity& Container, const flecs::entity& Item)
{
	const auto& ContainedBy = Item.get<FContainedBy>(Container);
	const int32 StackId = ContainedBy.Stack;
	
	if (const auto It = Stacks.find(StackId); It != Stacks.end())
	{
		if (It->second.Items.erase(Item) > 0)
		{
			Item.remove<FContainedBy>(Container);
		
			if (Type == EInventoryContainerType::AttachmentSlot)
			{
				// TODO: Detach
			}
		}

		if (It->second.Items.empty())
		{
			Stacks.erase(StackId);
		}
	}
}

bool FInventoryContainer::StoreItem(const flecs::entity& Container, const flecs::entity& Item,
	const FFlecsInventoryItemInfo& ItemInfo)
{
	const FFlecsInventoryItemSize ItemSize = ItemInfo.Size;

	UnrealFlecs::HashSet<int32> Occupied; Occupied.reserve(NumberOfColumns * NumberOfRows);

	for (auto&[StackId, Stack] : Stacks)
	{
		if (ItemInfo.AssetId == Stack.AssetId &&
			Stack.Items.size() < Stack.MaxStackAmount &&
			Stack.Size.Matches(ItemSize))
		{
			Stack.Items.insert(Item);
			Item.set(Container, FContainedBy{.Container = Name, .Stack = StackId});
			return true;
		}

		Occupied.insert(Stack.OccupiedSlots.begin(), Stack.OccupiedSlots.end());
	}

	UnrealFlecs::HashSet<int32> Slots; bool IsRotated = false;
	const int32 StackId = FindEmptyStack(ItemSize, Occupied, Slots, IsRotated);

	if (StackId != INDEX_NONE)
	{
		FContainerStack& Stack = Stacks[StackId];
	
		Stack.AssetId = ItemInfo.AssetId;
		Stack.Size = IsRotated ? ItemSize.GetRotated() : ItemSize;
		Stack.OccupiedSlots = std::move(Slots);
		Stack.Items.insert(Item);
		Stack.MaxStackAmount = ItemInfo.MaxStackAmount;
	
		Item.set(Container, FContainedBy{.Container = Name, .Stack = StackId});
		return true;
	}

	return false;
}

bool FInventoryContainer::EquipItem(const flecs::entity& Container, const flecs::entity& Item,
	const FFlecsInventoryItemInfo& ItemInfo)
{
	FContainerStack& Stack = Stacks[0];

	if (!Stack.Items.empty())
	{
		return false;
	}

	Stack.AssetId = ItemInfo.AssetId;
	Stack.Size = ItemInfo.Size;
	Stack.OccupiedSlots = {0};
	Stack.Items.insert(Item);

	Item.set(Container, FContainedBy{.Container = Name, .Stack = 0});
	return true;
}
