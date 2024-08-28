#pragma once

#include "FlecsTypeRegistry.h"
#include "FlecsCoreComponents.generated.h"

struct FFlecsAttachedTo;
struct FFlecsTransform;

struct UNREALFLECS_API FlecsTransformUtils
{
	using Local = FFlecsAttachedTo;
	using Global = FFlecsTransform;
	using TransformQuery = flecs::query<const Global, const Local, Global>;
	
	static void PropagateTransformUpdates(TransformQuery& Query, const flecs::entity& Parent);

	static void SetLocalTransform(TransformQuery& Query, const flecs::entity& Entity, const FTransform& NewTransform);
	static void SetGlobalLocation(TransformQuery& Query, const flecs::entity& Entity, const FVector& NewLocation);
	static void SetGlobalLocation(TransformQuery& Query, const flecs::entity& Entity, Global& Transform, const FVector& NewLocation);
	static void SetGlobalRotation(TransformQuery& Query, const flecs::entity& Entity, const FQuat& NewRotation);
	static void SetGlobalRotation(TransformQuery& Query, const flecs::entity& Entity, Global& Transform, const FQuat& NewRotation);
	static void SetGlobalTransform(TransformQuery& Query, const flecs::entity& Entity, const FTransform& NewTransform);
	static void SetGlobalTransform(TransformQuery& Query, const flecs::entity& Entity, Global& Transform, const FTransform& NewTransform);

	static void SetGlobalLocationAndRotation(TransformQuery& Query, const flecs::entity& Entity, Global& Transform,
		const FVector& NewLocation, const FQuat& NewRotation);
	
	static void AddGlobalOffset(TransformQuery& Query, const flecs::entity& Entity, Global& Transform, const FVector& Offset);
};

REG_FLECS_COMPONENT(FFlecsAttachedTo)
USTRUCT(BlueprintType)
struct FFlecsAttachedTo
{
	using TransformQuery = flecs::query<const FFlecsTransform, const FFlecsAttachedTo, FFlecsTransform>;
	
	GENERATED_BODY()

	const FTransform& Get() const { return Value; }
	
private:
	UPROPERTY(EditAnywhere)
	FName Socket = NAME_None;

	UPROPERTY(EditAnywhere)
	FTransform Value = FTransform::Identity;

	friend struct FFlecsTransform;
	friend struct FlecsTransformUtils;
};

REG_FLECS_COMPONENT(FFlecsTransform)
USTRUCT(BlueprintType)
struct FFlecsTransform
{
	using TransformQuery = flecs::query<const FFlecsTransform, const FFlecsAttachedTo, FFlecsTransform>;
	
	GENERATED_BODY()

	const FTransform& Get() const { return Value; }
	
private:
	UPROPERTY(EditAnywhere)
	FTransform Value = FTransform::Identity;

	friend struct FFlecsAttachedTo;
	friend struct FlecsTransformUtils;
};

UENUM(BlueprintType)
enum class EFlecsTransformUpdateMode : uint8
{
	FlecsToUWorld,
	UWorldToFlecs
};