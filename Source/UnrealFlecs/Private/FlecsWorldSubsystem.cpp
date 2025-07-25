// Copyright © 2025 Byt3Mage. All Rights Reserved.


#include "FlecsWorldSubsystem.h"

void UFlecsWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FlecsWorld = std::make_unique<flecs::world>();
	OnCreateFlecsWorld(*FlecsWorld);
}

void UFlecsWorldSubsystem::Deinitialize()
{
	FlecsWorld = nullptr;
	Super::Deinitialize();
}

void UFlecsWorldSubsystem::OnCreateFlecsWorld(flecs::world& Flecs)
{
}

void UFlecsWorldSubsystem::OnDestroyFlecsWorld(flecs::world& Flecs)
{
}