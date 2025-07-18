// Copyright © 2025 ClownWare Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItem.h"
#include "Engine/DataAsset.h"
#include "ItemDescription.generated.h"

/**
 * Item description asset
 */
UCLASS()
class FLECSINVENTORY_API UFlecsItemDescription : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag Tag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Name = "";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FString Description = "";

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FFlecsInventoryItemSize Size  = 1;
};
