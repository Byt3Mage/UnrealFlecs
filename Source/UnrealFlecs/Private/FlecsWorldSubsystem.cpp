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
	OnDestroyFlecsWorld(*FlecsWorld);
	FlecsWorld = nullptr;
	Super::Deinitialize();
}

const flecs::world& UFlecsWorldSubsystem::GetFlecsWorld() const
{
	return *FlecsWorld;
}

flecs::world& UFlecsWorldSubsystem::GetFlecsWorld()
{
	return *FlecsWorld;
}

void UFlecsWorldSubsystem::TickFlecsWorld(const float DeltaTime) const
{
	FlecsWorld->progress(DeltaTime);
}

void UFlecsWorldSubsystem::OnCreateFlecsWorld(flecs::world& Flecs)
{
}

void UFlecsWorldSubsystem::OnDestroyFlecsWorld(flecs::world& Flecs)
{
}