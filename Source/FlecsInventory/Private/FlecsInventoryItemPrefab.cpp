// Copyright © 2025 ClownWare Games. All Rights Reserved.

#include "FlecsInventoryItemPrefab.h"

void UFlecsInventoryItemPrefab::SetInstanceComponents(flecs::entity& Entity) const
{
	Super::SetInstanceComponents(Entity);

	Entity.set(FFlecsInventoryItemInfo 
	{
		.AssetId = GetPrimaryAssetId(),
		.Tag = Tag,
		.Size = Size,
		.MaxStackAmount = MaxStackAmount,
	});

	for (auto Archetype : Archetypes)
	{
		if (const auto* Ptr = Archetype.GetPtr())
		{
			Ptr->SetArchetypeOnEntity(Entity);
		}
	}
}