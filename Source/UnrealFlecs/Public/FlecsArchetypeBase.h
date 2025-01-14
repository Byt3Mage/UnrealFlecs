#pragma once
#include "FlecsTypeRegistry.h"
#include "InstancedStruct.h"
#include "StructUtils/InstancedStruct.h"

#include "FlecsArchetypeBase.generated.h"

USTRUCT(BlueprintType)
struct FFlecsArchetypeBase
{
	GENERATED_BODY()
	
	virtual ~FFlecsArchetypeBase() = default;
	virtual void SetArchetypeOnEntity(flecs::entity& Entity) const {}
};

USTRUCT(BlueprintType)
struct FFlecsAssortedComponents : public FFlecsArchetypeBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FInstancedStruct> Components;

	virtual void SetArchetypeOnEntity(flecs::entity& Entity) const override
	{
		for (const FInstancedStruct& Comp : Components)
		{
			if (auto* Fn = FFlecsTypeRegistry::Get().find_set_fn(Comp.GetScriptStruct()))
			{
				(*Fn)(Entity, Comp);
			}
		}
	}
};

USTRUCT(BlueprintType)
struct FFlecsAssortedTags : public FFlecsArchetypeBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TObjectPtr<const UScriptStruct>> Tags;

	virtual void SetArchetypeOnEntity(flecs::entity& Entity) const override
	{
		for (const auto& Tag : Tags)
		{
			if (auto* Fn = FFlecsTypeRegistry::Get().find_add_fn(Tag))
			{
				(*Fn)(Entity);
			}
		}
	}
};