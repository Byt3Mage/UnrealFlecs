#include "FlecsCoreUEModule.h"
#include <set>
#include <sstream>
#include <string>
#include "flecs.h"
#include "FlecsCoreComponents.h"
#include "GameplayTagContainer.h"

FlecsCoreUEModule::FlecsCoreUEModule(const flecs::world& FlecsWorld)
{
	FlecsWorld.component<FString>()
	.opaque(flecs::String)
	.serialize([](const flecs::serializer* s, const FString* Data)
	{
		const char* Ptr = StringCast<ANSICHAR>(GetData(*Data)).Get();
		return s->value(flecs::String, &Ptr);
	})
	.assign_string([](FString* Dst, const char* value)
	{
		*Dst = value;
	});
	
	FlecsWorld.component<FTransform>("Transform")
	.opaque(flecs::String)
	.serialize([](const flecs::serializer* s, const FTransform* Data)
	{
		FString Transform = Data->ToString();
		const auto Src = StringCast<ANSICHAR>(GetData(Transform));
		const char* Ptr = Src.Get();
		return s->value(flecs::String, &Ptr);
	})
	.assign_string([](FTransform* Dst, const char* value)
	{
		Dst->InitFromString(value);
	});

	FlecsWorld.component<FName>()
	.opaque(flecs::String)
	.serialize([](const flecs::serializer* s, const FName* Data)
	{
		const char* Ptr = StringCast<ANSICHAR>(GetData(Data->ToString())).Get();
		return s->value(flecs::String, &Ptr);
	})
	.assign_string([](FName* Dst, const char* value)
	{
		*Dst = value;
	});
	
	FlecsWorld.component<FQuat>()
	.member<FQuat::FReal>("X")
	.member<FQuat::FReal>("Y")
	.member<FQuat::FReal>("Z")
	.member<FQuat::FReal>("W");

	FlecsWorld.component<FVector>()
	.member<FVector::FReal>("X")
	.member<FVector::FReal>("Y")
	.member<FVector::FReal>("Z");

	FlecsWorld.component<FIntPoint>()
	.member<FIntPoint::IntType>("X")
	.member<FIntPoint::IntType>("Y");
	
	FlecsWorld.component<FGuid>()
	.member<uint32>("A")
	.member<uint32>("B")
	.member<uint32>("C")
	.member<uint32>("D");

	FlecsWorld.component<FGameplayTag>()
	.member<FName>("TagName");

	FlecsWorld.component<FFlecsAttachedTo>()
	.member<FName>("SocketName")
	.member<FTransform>("RelativeTransform")
	.add(flecs::Exclusive)
	.add(flecs::Acyclic)
	.add(flecs::Traversable);
	
	// Register reflection for std::vector<int>
	flecs::component<std::vector<int32>> VectorComp = FlecsWorld.component<std::vector<int32>>()
	.opaque(std_vector_support<int32>);

	FlecsWorld.component<std::set<int32>>()
	.opaque(VectorComp.id())
	.serialize([](const flecs::serializer *s, const std::set<int32>* data) 
	{
		return s->value(std::vector(data->begin(), data->end()));
	})
	.assign_string([](std::set<int32>* Dst, const char* value)
	{
		std::string_view Source(value); Source = Source.substr(1, Source.length() - 2); // Remove the brackets
        std::stringstream Stream(std::string{Source});
        std::string Item;

		Dst->clear();
		
		while (std::getline(Stream, Item, ','))
		{
			Dst->insert(std::stoi(Item));
		}
	});

	FlecsWorld.component<FFlecsAttachedTo>()
	.member<FName>("Socket")
	.member<FTransform>("Value")
	.add(flecs::Traversable)
	.add(flecs::Exclusive)
	.add(flecs::Relationship);

	FlecsWorld.component<FFlecsTransform>()
	.member<FTransform>("Value");
}