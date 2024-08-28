#pragma once

#include <vector>

#include "flecs.h"

namespace flecs
{
	struct world;
}

struct UNREALFLECS_API FlecsCoreUEModule
{
	FlecsCoreUEModule(const flecs::world& FlecsWorld);

	// Reusable reflection support for std::vector
	template <typename Elem, typename Vector = std::vector<Elem>> 
	static flecs::opaque<Vector, Elem> std_vector_support(flecs::world& world) 
	{
		return flecs::opaque<Vector, Elem>().as_type(world.vector<Elem>())

		// Forward elements of std::vector value to serializer
		.serialize([](const flecs::serializer *s, const Vector *data)
		{
			for (const auto& el : *data)
			{
				s->value(el);
			}

			return 0;
		})

		// Return vector count
		.count([](const Vector *data)
		{
			return data->size();
		})

		// Resize contents of vector
		.resize([](Vector *data, size_t size)
		{
			data->resize(size);
		})

		// Ensure element exists, return pointer
		.ensure_element([](Vector *data, size_t elem)
		{
			if (data->size() <= elem) 
			{
				data->resize(elem + 1);
			}

			return &data->data()[elem];
		});
	}

	// Reusable reflection support for TArray
	template <typename Elem, typename Vector = TArray<Elem>> 
	static flecs::opaque<Vector, Elem> TArraySupport(flecs::world& world) 
	{
		return flecs::opaque<Vector, Elem>().as_type(world.vector<Elem>())

		// Forward elements of std::vector value to serializer
		.serialize([](const flecs::serializer *s, const Vector *data)
		{
			for (const auto& el : *data)
			{
				s->value(el);
			}

			return 0;
		})

		// Return vector count
		.count([](const Vector *data)
		{
			return data->Num();
		})

		// Resize contents of vector
		.resize([](Vector *data, size_t size)
		{
			data->ResizeTo(size);
		})

		// Ensure element exists, return pointer
		.ensure_element([](Vector *data, size_t elem)
		{
			if (data->Num() <= elem) 
			{
				data->ResizeTo(elem + 1);
			}

			return &data->GetData()[elem];
		});
	}
};