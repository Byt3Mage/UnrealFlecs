#pragma once

#include "FlecsTypeRegistry.h"
#include "FlecsQuery.generated.h"

UENUM(BlueprintType)
enum class EFlecsQueryTermType : uint8
{
	Component,
	Pair,
	AnyObject,
	WildcardObject,
	AnyRelationship,
	WildcardRelationship
};

UENUM(BlueprintType)
enum class EFlecsQueryTermAccess : uint8
{
	None,
	In,
	Out,
	InOut
};

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsQueryTerm
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EFlecsQueryTermType Type = EFlecsQueryTermType::Component;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EFlecsQueryTermAccess Access = EFlecsQueryTermAccess::None;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="Type == EFlecsQueryTermType::Component", EditConditionHides))
	TObjectPtr<const UScriptStruct> Component;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(
		EditCondition="Type == EFlecsQueryTermType::Pair || Type == EFlecsQueryTermType::AnyObject || Type == EFlecsQueryTermType::WildcardObject",
		EditConditionHides))
	TObjectPtr<const UScriptStruct> Relationship;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(
		EditCondition="Type == EFlecsQueryTermType::Pair || Type == EFlecsQueryTermType::AnyRelationship || Type == EFlecsQueryTermType::WildcardRelationship",
		EditConditionHides))
	TObjectPtr<const UScriptStruct> Object;
	
	flecs::id GetID(const flecs::world& FlecsWorld) const
	{
		FFlecsTypeRegistry& Registry = FFlecsTypeRegistry::Get();

		flecs::id CompID = Registry.find_id(Component, FlecsWorld);
		flecs::id RelID = Registry.find_id(Relationship, FlecsWorld);
		flecs::id ObjID = Registry.find_id(Object, FlecsWorld);

		switch (Type)
		{
		case EFlecsQueryTermType::Component: return CompID;
		case EFlecsQueryTermType::Pair: return FlecsWorld.pair(RelID, ObjID);
		case EFlecsQueryTermType::AnyObject: return FlecsWorld.pair(RelID, flecs::Any);
		case EFlecsQueryTermType::WildcardObject: return FlecsWorld.pair(RelID, flecs::Wildcard);
		case EFlecsQueryTermType::AnyRelationship: return FlecsWorld.pair(flecs::Any, ObjID);
		case EFlecsQueryTermType::WildcardRelationship: return FlecsWorld.pair(flecs::Wildcard, ObjID);
		}

		return flecs::id();
	}
	
	void AddToQuery(const flecs::world& FlecsWorld, flecs::query_builder<>& QueryBuilder) const
	{
		flecs::id TermID = GetID(FlecsWorld);
		
		if (TermID != 0)
		{
			QueryBuilder.with(TermID);

			switch (Access)
			{
			case EFlecsQueryTermAccess::None: QueryBuilder.inout_none(); break;
			case EFlecsQueryTermAccess::In: QueryBuilder.in(); break;
			case EFlecsQueryTermAccess::Out: QueryBuilder.out(); break;
			case EFlecsQueryTermAccess::InOut: QueryBuilder.inout(); break;
			default: break;
			}
		}
	}
	
	FString ToString(const flecs::world& FlecsWorld) const
	{
		const flecs::id ID = GetID(FlecsWorld);
		
		return ID != 0 ? ID.str().c_str() : "";
	}
};