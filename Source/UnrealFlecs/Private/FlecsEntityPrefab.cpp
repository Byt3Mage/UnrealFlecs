// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsEntityPrefab.h"

flecs::entity UFlecsEntityPrefab::CreateInstance(const flecs::world& FlecsWorld, FFlecsPrefabRegistry& Registry) const
{
	flecs::entity Entity;

	if (flecs::entity Prefab = Registry.GetOrCreatePrefab(FlecsWorld, this))
	{
		Entity = FlecsWorld.entity().is_a(Prefab);
		// Set overriden components
		SetInstanceComponents(Entity);
	}
	
	return Entity;
}

TArray<flecs::entity> UFlecsEntityPrefab::BatchCreateInstances(const flecs::world& FlecsWorld,
	FFlecsPrefabRegistry& Registry, const int32 Count) const
{
	TArray<flecs::entity> Instances;
	
	if (flecs::entity Prefab = Registry.GetOrCreatePrefab(FlecsWorld, this))
	{
		Instances.Reserve(Count);
		
		for (int i = 0; i < Count; ++i)
		{
			flecs::entity& Instance = Instances.Add_GetRef(FlecsWorld.entity().is_a(Prefab));
			SetInstanceComponents(Instance);
		}
	}

	return Instances;
}

TArray<FFlecsEntityHandle> UFlecsEntityPrefab::BatchCreateInstanceHandles(const flecs::world& FlecsWorld,
	FFlecsPrefabRegistry& Registry, const int32 Count) const
{
	TArray<FFlecsEntityHandle> Instances;
	
	if (flecs::entity Prefab = Registry.GetOrCreatePrefab(FlecsWorld, this))
	{
		Instances.Reserve(Count);
		
		for (int i = 0; i < Count; ++i)
		{
			flecs::entity Instance = FlecsWorld.entity().is_a(Prefab);
			SetInstanceComponents(Instance);
			Instances.Emplace(Instance);
		}
	}

	return Instances;
}