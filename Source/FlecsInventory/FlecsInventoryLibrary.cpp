// Copyright © 2025 ClownWare Games. All Rights Reserved.


#include "FlecsInventoryLibrary.h"

#include "FlecsInventoryContainer.h"

bool UFlecsInventoryLibrary::AddItemToInventory(const FFlecsEntityHandle Item, const FFlecsEntityHandle Inventory)
{
	return FlecsInventory::AddItemToInventory(Item, Inventory);
}

bool UFlecsInventoryLibrary::AddItemToContainer(const FFlecsEntityHandle Item, const FFlecsEntityHandle Inventory,
                                                const FName ContainerName)
{
	return FlecsInventory::AddItemToInventory(Item, Inventory, ContainerName);
}