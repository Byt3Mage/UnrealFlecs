// Copyright © 2025 Byt3Mage. All Rights Reserved.

#pragma once

#include <memory>

#include "flecs.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsWorldSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UNREALFLECS_API UFlecsWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	/**
	 * Intentionally prevented from being created.
	 * Subclasses should refer to UWorldSubsystem for the proper implementation of this function.
	 */
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return false; }
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	flecs::world& GetFlecsWorld();
	
	const flecs::world& GetFlecsWorld() const;
	
	/**
	 * Call on Tick to progress the flecs world pipeline.
	 * @param DeltaTime Update rate of flecs world. Pass 0 to make flecs use internal DeltaTime.
	 */
	FORCEINLINE void TickFlecsWorld(const float DeltaTime) const;
	
	/**
	 * Called after the flecs world is created.
	 * Overriding classes can import modules here.
	 */
	virtual void OnCreateFlecsWorld(flecs::world& Flecs);

	/**
	 * Called before the flecs world is destroyed
	 * Overriding classes should run cleanup here.
	 */
	virtual void OnDestroyFlecsWorld(flecs::world& Flecs);
	
private:
	std::unique_ptr<flecs::world> FlecsWorld = nullptr;
};
