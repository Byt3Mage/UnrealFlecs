// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsEntityPrefab.h"

flecs::entity UFlecsEntityPrefab::CreateInstance(const flecs::world& FlecsWorld) const
{
	flecs::entity Entity;

	auto* Registry = FlecsWorld.get_mut<FFlecsPrefabRegistry>();

	if (Registry)
	{
		if (flecs::entity Prefab = Registry->GetOrCreatePrefab(FlecsWorld, this))
		{
			Entity = FlecsWorld.entity().is_a(Prefab);

			// Set overriden components
			SetInstanceComponents(Entity);
		}
	}
	
	return Entity;
}

TArray<flecs::entity> UFlecsEntityPrefab::BatchCreateInstances(const flecs::world& FlecsWorld, const int32 Count,
	const FTransform& Transform, const bool bWithTransform) const
{
	TArray<flecs::entity> Instances;
	
	if (auto* Registry = FlecsWorld.get_mut<FFlecsPrefabRegistry>())
	{
		if (flecs::entity Prefab = Registry->GetOrCreatePrefab(FlecsWorld, this))
		{
			 Instances.Reserve(Count);
			
			if (bWithTransform)
			{
				for (int i = 0; i < Count; ++i)
				{
					flecs::entity Instance = Instances.Add_GetRef(FlecsWorld.entity().is_a(Prefab));
					SetInstanceComponents(Instance);
					Instance.set(Transform);
				}
			}
			else
			{
				for (int i = 0; i < Count; ++i)
				{
					flecs::entity Instance = Instances.Add_GetRef(FlecsWorld.entity().is_a(Prefab));
					SetInstanceComponents(Instance);
				}
			}
		}
	}

	return Instances;
}

TArray<FFlecsEntityHandle> UFlecsEntityPrefab::BatchCreateInstanceHandles(const flecs::world& FlecsWorld, const int32 Count,
	const FTransform& Transform, const bool bWithTransform) const
{
	TArray<FFlecsEntityHandle> Instances;
	
	if (auto* Registry = FlecsWorld.get_mut<FFlecsPrefabRegistry>())
	{
		if (flecs::entity Prefab = Registry->GetOrCreatePrefab(FlecsWorld, this))
		{
			Instances.Reserve(Count);
			
			if (bWithTransform)
			{
				for (int i = 0; i < Count; ++i)
				{
					flecs::entity Instance = FlecsWorld.entity().is_a(Prefab);
					SetInstanceComponents(Instance);
					Instance.set(Transform);
					
					Instances.Emplace(Instance);
				}
			}
			else
			{
				for (int i = 0; i < Count; ++i)
				{
					flecs::entity Instance = FlecsWorld.entity().is_a(Prefab);
					SetInstanceComponents(Instance);
					Instances.Emplace(Instance);
				}
			}
		}
	}

	return Instances;
}

TArray<flecs::entity> UFlecsEntityPrefab::BatchCreateWithTransforms(const flecs::world& FlecsWorld,
	const TArray<FTransform>& Transforms) const
{
	TArray<flecs::entity> Instances;
	
	if (auto* Registry = FlecsWorld.get_mut<FFlecsPrefabRegistry>())
	{
		if (flecs::entity Prefab = Registry->GetOrCreatePrefab(FlecsWorld, this))
		{
			Instances.Reserve(Transforms.Num());

			for (const FTransform& Transform : Transforms)
			{
				flecs::entity Instance = FlecsWorld.entity().is_a(Prefab);
				SetInstanceComponents(Instance);
				Instance.set(Transform);
					
				Instances.Add(Instance);
			}
		}
	}

	return Instances;
}

TArray<FFlecsEntityHandle> UFlecsEntityPrefab::BatchCreateHandlesWithTransforms(const flecs::world& FlecsWorld,
	const TArray<FTransform>& Transforms) const
{
	TArray<FFlecsEntityHandle> Instances;
	
	if (auto* Registry = FlecsWorld.get_mut<FFlecsPrefabRegistry>())
	{
		if (flecs::entity Prefab = Registry->GetOrCreatePrefab(FlecsWorld, this))
		{
			Instances.Reserve(Transforms.Num());

			for (const FTransform& Transform : Transforms)
			{
				flecs::entity Instance = FlecsWorld.entity().is_a(Prefab);
				SetInstanceComponents(Instance);
				Instance.set(Transform);
					
				Instances.Emplace(Instance);
			}
		}
	}

	return Instances;
}