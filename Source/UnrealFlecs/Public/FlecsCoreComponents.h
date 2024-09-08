#pragma once

#include "FlecsTypeRegistry.h"
#include "FlecsCoreComponents.generated.h"

struct FFlecsAttachedTo;
struct FFlecsTransform;

struct UNREALFLECS_API FlecsTransformUtils
{
	using TransformQuery = flecs::query<const FFlecsTransform, const FFlecsAttachedTo, FFlecsTransform>;
	
	static void PropagateTransformUpdates(const TransformQuery& Query, const flecs::entity& Parent);

	static void SetLocalTransform(const TransformQuery& Query, const flecs::entity& Entity, const FTransform& NewTransform);
	static void SetGlobalLocation(const TransformQuery& Query, const flecs::entity& Entity, const FVector& NewLocation);
	static void SetGlobalLocation(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FVector& NewLocation);
	static void SetGlobalRotation(const TransformQuery& Query, const flecs::entity& Entity, const FQuat& NewRotation);
	static void SetGlobalRotation(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FQuat& NewRotation);
	static void SetGlobalTransform(const TransformQuery& Query, const flecs::entity& Entity, const FTransform& NewTransform);
	static void SetGlobalTransform(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FTransform& NewTransform);

	static void SetLocationAndRotation(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform,
		const FVector& NewLocation, const FQuat& NewRotation);
	
	static void AddGlobalOffset(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FVector& Offset);
	
	static void AttachEntityTo(const TransformQuery& Query, const flecs::entity& Entity, const flecs::entity& Parent, const FTransform& RelativeTransform = FTransform::Identity);

private:
	static void UpdateLocalTransform(const flecs::entity& Entity, const FTransform& Transform);
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