#pragma once
#include "GameplayTagContainer.h"
#include "FlecsInventoryComponents.generated.h"

namespace flecs
{
	struct world;
}

struct FlecsInventoryComponents
{
	FlecsInventoryComponents(flecs::world& FlecsWorld);
};

USTRUCT(BlueprintType)
struct FLECSINVENTORY_API FFlecsInventoryItemSize
{
	GENERATED_BODY()

	FFlecsInventoryItemSize() = default;
	FFlecsInventoryItemSize(const uint8 Size) : Width(Size), Height(Size) {}
	FFlecsInventoryItemSize(const uint8 InWidth, const uint8 InHeight) : Width(InWidth), Height(InHeight) {}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Width = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 Height = 1;
	
	/** @return the item size with the width and height swapped */
	FFlecsInventoryItemSize GetRotated() const { return FFlecsInventoryItemSize(Height, Width); }
	
	/**
	 * Checks to see if the other size matches this size with or without rotation.
	 * @param Other Other size to compare to
	 * @return true if the other size matches
	 */
	bool Matches(const FFlecsInventoryItemSize& Other) const
	{
		return (Width == Other.Width && Height == Other.Height) || (Width == Other.Height && Height == Other.Width);
	}

	bool operator==(const FFlecsInventoryItemSize& Other) const { return Width == Other.Width && Height == Other.Height; }
};

USTRUCT(BlueprintType)
struct FLECSINVENTORY_API FFlecsInventoryItemInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FPrimaryAssetId AssetId = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag Tag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FFlecsInventoryItemSize Size = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxStackAmount = 1;
};

USTRUCT(BlueprintType)
struct FLECSINVENTORY_API FContainedBy
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Container = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 StackId = INDEX_NONE;
};