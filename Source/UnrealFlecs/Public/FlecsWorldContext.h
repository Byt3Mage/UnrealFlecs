#pragma once

#include "flecs.h"
#include "FlecsCoreComponents.h"

struct FlecsWorldContext
{
	UWorld* UnrealWorld = nullptr;

	flecs::query<const FFlecsTransform, const FFlecsAttachedTo, FFlecsTransform> TransformQuery;
};