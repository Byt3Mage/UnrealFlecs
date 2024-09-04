#pragma once

#include "flecs.h"

namespace flecs_utils
{
	template <typename T>
	T& ensure_component(const flecs::entity& entity)
	{
		flecs::world_t* flecs_world = entity.world().c_ptr();
		
		auto comp_id = flecs::_::type<T>::id(flecs_world);
		ecs_assert(flecs::_::type<T>::size() != 0, ECS_INVALID_PARAMETER, "operation invalid for empty type");
		return *static_cast<T*>(ecs_ensure_id(flecs_world, entity.id(), comp_id));
	}

	template <typename First>
	First& ensure_component(const flecs::entity& entity, const flecs::entity_t second)
	{
		flecs::world_t* flecs_world = entity.world().c_ptr();
		
		auto comp_id = flecs::_::type<First>::id(flecs_world);
		ecs_assert(flecs::_::type<First>::size() != 0, ECS_INVALID_PARAMETER, "operation invalid for empty type");
		return *static_cast<First*>(ecs_ensure_id(flecs_world, entity.id(), ecs_pair(comp_id, second)));
	}

	template<typename First>
	std::tuple<flecs::entity, const First*> get_pair(const flecs::entity& entity)
	{
		flecs::world flecs_world = entity.world();
		flecs::table_range r = entity.range();
		flecs::id_t pair_out;
		int index = ecs_search(flecs_world, r, flecs_world.pair<First>(flecs::Wildcard), &pair_out);

		if (index != -1)
		{
			flecs::entity second = flecs_world.id(pair_out).second();
			const First* component = static_cast<First*>(r.get_column(index));

			return {second, component};
		}

		return {flecs::entity(), nullptr};
	}

	template<typename First>
	std::tuple<flecs::entity, First*> get_pair_mut(const flecs::entity& entity)
	{
		flecs::world flecs_world = entity.world();
		flecs::table_range r = entity.range();
		flecs::id_t pair_out;
		int index = -1;
			//ecs_search(flecs_world, r, flecs_world.pair<First>(flecs::Wildcard), &pair_out);

		if (index != -1)
		{
			flecs::entity second = flecs_world.id(pair_out).second();
			First* component = static_cast<First*>(r.get_column(index));

			return {second, component};
		}

		return {flecs::entity(), nullptr};
	}
}