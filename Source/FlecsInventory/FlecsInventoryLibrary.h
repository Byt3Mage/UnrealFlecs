// Copyright © 2025 ClownWare Games. All Rights Reserved.

#pragma once

#include "FlecsEntityHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlecsInventoryLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FLECSINVENTORY_API UFlecsInventoryLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static bool AddItemToInventory(FFlecsEntityHandle Item, FFlecsEntityHandle Inventory);
	
	UFUNCTION(BlueprintCallable)
	static bool AddItemToContainer(FFlecsEntityHandle Item, FFlecsEntityHandle Inventory, FName ContainerName);
};
