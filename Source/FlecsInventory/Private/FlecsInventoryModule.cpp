#include "FlecsInventoryModule.h"

#include "flecs.h"
#include "InventoryItem.h"

FlecsInventoryModule::FlecsInventoryModule(const flecs::world& FlecsWorld)
{
	FlecsWorld.component<FFlecsInventoryItemSize>()
	.member("Width", &FFlecsInventoryItemSize::Width)
	.member("Height", &FFlecsInventoryItemSize::Height);

	FlecsWorld.component<FFlecsInventoryItemInfo>();
	
	FlecsWorld.component<FContainedBy>()
	.member("Container", &FContainedBy::Container)
	.member("Stack", &FContainedBy::Stack)
#ifdef DONT_FRAGMENT_CONTAINEDBY
	.add(flecs::EcsDontFragment)
#endif
	.add(flecs::Exclusive)
	.add(flecs::Traversable);
}
