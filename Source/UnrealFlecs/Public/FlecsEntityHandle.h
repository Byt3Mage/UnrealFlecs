#pragma once

#include "flecs.h"
#include "FlecsEntityHandle.generated.h"

USTRUCT(BlueprintType)
struct FFlecsEntityHandle
{
	GENERATED_BODY()

	FFlecsEntityHandle() = default;
	
	//FFlecsEntityHandle(const uint64 id) : Value(id), FlecsWorld(nullptr) {}
	FFlecsEntityHandle(const flecs::entity& Entity)
	: FlecsId(Entity)
	, FlecsWorld(Entity.world())
	{}

	operator uint64() const
	{
		return FlecsId;
	}

	operator flecs::entity() const
	{
		return flecs::entity(FlecsWorld, FlecsId);
	}
	
	bool IsValid() const
	{
		return FlecsWorld && ecs_is_valid(FlecsWorld, FlecsId);
	}

	bool IsAlive() const
	{
		return FlecsWorld && ecs_is_alive(FlecsWorld, FlecsId);
	}

	FORCEINLINE flecs::entity GetEntity(const flecs::world& World) const
	{
		return flecs::entity(World, FlecsId);
	}

	template<typename T>
	const T* Get() const
	{
		return flecs::entity(FlecsWorld, FlecsId).get<T>();
	}

	template<typename T>
	T* GetMut() const
	{
		return flecs::entity(FlecsWorld, FlecsId).get_mut<T>();
	}

	template<typename T>
	flecs::ref<T> GetRef() const
	{
		return flecs::entity(FlecsWorld, FlecsId).get_ref<T>();
	}

	template<typename T>
	void Set(const T& Value) const
	{
		flecs::set<T>(FlecsWorld, Value, Value);
	}

	template<typename T>
	void Set(T&& Value) const
	{
		flecs::set<T>(FlecsWorld, FlecsId, FLECS_FWD(Value));
	}
	
	template<typename T>
	bool Has() const
	{
		return flecs::entity(FlecsWorld, FlecsId).has<T>();
	}

	template<typename T>
	bool Has(const flecs::entity& Second) const
	{
		return flecs::entity(FlecsWorld, FlecsId).has<T>(Second);
	}

	template <typename First, typename Second>
	bool Has() const
	{
		return flecs::entity(FlecsWorld, FlecsId).has<First, Second>();
	}

	flecs::world World() const
	{
		return flecs::world(FlecsWorld);
	}
	
private:
	UPROPERTY()
	uint64 FlecsId = 0;

	flecs::world_t *FlecsWorld;
};