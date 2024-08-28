#pragma once

#include "flecs.h"

namespace flecs_utils
{
	using namespace flecs;
	
	template <typename T>
	T& ensure_component(const entity& entity)
	{
		world_t* FlecsWorld = entity.world().c_ptr();
		
		auto comp_id = _::type<T>::id(FlecsWorld);
		ecs_assert(_::type<T>::size() != 0, ECS_INVALID_PARAMETER, "operation invalid for empty type");
		return *static_cast<T*>(ecs_ensure_id(FlecsWorld, entity.id(), comp_id));
	}

	template <typename First>
	First& ensure_component(const entity& entity, const entity_t second)
	{
		world_t* FlecsWorld = entity.world().c_ptr();
		
		auto comp_id = _::type<First>::id(FlecsWorld);
		ecs_assert(_::type<First>::size() != 0, ECS_INVALID_PARAMETER, "operation invalid for empty type");
		return *static_cast<First*>(ecs_ensure_id(FlecsWorld, entity.id(), ecs_pair(comp_id, second)));
	}
}