#pragma once

#include "flecs.h"
#include "FlecsEntityHandle.generated.h"

USTRUCT(BlueprintType)
struct FFlecsEntityHandle
{
	GENERATED_BODY()

	FFlecsEntityHandle() = default;
	
	//FFlecsEntityHandle(const uint64 id) : Value(id), FlecsWorld(nullptr) {}
	FFlecsEntityHandle(const flecs::entity& Entity) : Value(Entity), FlecsWorld(Entity.world()){}

	operator uint64() const
	{
		return Value;
	}

	bool IsValid() const
	{
		return ecs_is_valid(FlecsWorld, Value);
	}

	bool IsAlive() const
	{
		return ecs_is_alive(FlecsWorld, Value);
	}

	FORCEINLINE flecs::entity GetEntity() const
	{
		return flecs::entity(FlecsWorld, Value);
	}

	FORCEINLINE flecs::entity GetEntity(const flecs::world& World) const
	{
		return flecs::entity(World, Value);
	}

	template<typename T>
	const T* Get() const
	{
		return flecs::entity(FlecsWorld, Value).get<T>();
	}

	template<typename T>
	T* GetMut() const
	{
		return flecs::entity(FlecsWorld, Value).get_mut<T>();
	}

	template<typename T>
	flecs::ref<T> GetRef() const
	{
		return flecs::entity(FlecsWorld, Value).get_ref<T>();
	}

	template<typename T>
	void Set(const T& Value) const
	{
		flecs::entity(FlecsWorld, Value).set<T>(Value);
	}

	template<typename T>
	void Set(T&& Value) const
	{
		flecs::entity(FlecsWorld, Value).set<T>(FLECS_FWD(Value));
	}
	
	template<typename T>
	bool Has() const
	{
		return flecs::entity(FlecsWorld, Value).has<T>();
	}

	template<typename T>
	bool Has(const flecs::entity& Second) const
	{
		return flecs::entity(FlecsWorld, Value).has<T>(Second);
	}

	template <typename First, typename Second>
	bool Has() const
	{
		return flecs::entity(FlecsWorld, Value).has<First, Second>();
	}
	
private:
	UPROPERTY()
	uint64 Value = 0;

	flecs::world_t* FlecsWorld;
};