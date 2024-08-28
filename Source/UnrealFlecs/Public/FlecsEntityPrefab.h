// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "flecs.h"
#include "FlecsArchetypeBase.h"
#include "FlecsEntityHandle.h"
#include "FlecsEntityPrefab.generated.h"

struct FFlecsPrefabRegistry;

UCLASS(Abstract, BlueprintType)
class UNREALFLECS_API UFlecsEntityPrefab : public UPrimaryDataAsset
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(BaseStruct="/Script/UnrealFlecs.FlecsArchetypeBase"))
	TArray<FInstancedStruct> SharedArchetypes;
	
public:
	virtual void SetInstanceComponents(flecs::entity& Entity) const {}
	
	flecs::entity CreateInstance(const flecs::world& FlecsWorld, FFlecsPrefabRegistry& Registry) const;

	TArray<flecs::entity> BatchCreateInstances(const flecs::world& FlecsWorld, FFlecsPrefabRegistry& Registry, int32 Count) const;

	TArray<FFlecsEntityHandle> BatchCreateInstanceHandles(const flecs::world& FlecsWorld, FFlecsPrefabRegistry& Registry, const int32 Count) const;
	
protected:
	virtual flecs::entity CreatePrefab(const flecs::world& FlecsWorld) const
	{
		flecs::entity Prefab = FlecsWorld.prefab();
		
		for (const auto& Archetype : SharedArchetypes)
		{
			if (const auto* Trait = Archetype.GetPtr<FFlecsArchetypeBase>())
			{
				Trait->SetArchetypeOnEntity(Prefab);
			}
		}
		
		return Prefab;
	}
	
	friend struct FFlecsPrefabRegistry;
};

struct FFlecsPrefabRegistry
{
	using FAssetHandle = TSoftObjectPtr<const UFlecsEntityPrefab>;
	using FPrefabMap = TMap<FAssetHandle, flecs::entity>;

	FFlecsPrefabRegistry()
	{
		Prefabs.Reserve(1000);
	}
	
	flecs::entity GetOrCreatePrefab(const flecs::world& FlecsWorld, const UFlecsEntityPrefab* Asset)
	{
		if (!IsValid(Asset)) { return flecs::entity(); }

		const FAssetHandle AssetHandle = Asset;
		flecs::entity Prefab;
		
		if (flecs::entity* Found = Prefabs.Find(AssetHandle))
		{
			Prefab = *Found;
		}

		if (!Prefab.is_alive())
		{
			// Cleanup dead Prefab and create new one
			Prefabs.Remove(AssetHandle);
			
			Prefab = Asset->CreatePrefab(FlecsWorld);

			if (Prefab.is_alive())
			{
				Prefabs.Add(AssetHandle, Prefab);
			}
		}

		return Prefab;
	}

private:
	FPrefabMap Prefabs;
};