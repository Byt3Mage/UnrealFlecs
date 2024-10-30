#include "FlecsCoreComponents.h"
#include "FlecsUtils.h"

void FlecsTransformUtils::PropagateTransformUpdate(const TransformQuery& Query, const flecs::entity& Parent)
{
	Query.iter(Parent)
	.set_group(Parent)
	.each([&Query](const flecs::entity child, const FFlecsTransform& parent_transform,
		const FFlecsAttachedTo& attachment, FFlecsTransform& child_transform)
	{
		child_transform.Value = attachment.Value * parent_transform.Value;
		PropagateTransformUpdate(Query, child);
	});
}

void FlecsTransformUtils::SetLocalTransform(const TransformQuery& Query, const flecs::entity& Entity, const FTransform& NewTransform)
{
	auto[parent, attachment] = flecs_utils::get_pair_mut<FFlecsAttachedTo>(Entity);
	
	if (attachment)
	{
		auto* parent_transform = parent.get<FFlecsTransform>();
		attachment->Value = NewTransform;

		// Update global transform
		if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
		{
			global_transform->Value = NewTransform * parent_transform->Value;
			PropagateTransformUpdate(Query, Entity);
		}
	}
	// No local transform found, set global if available
	else if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		global_transform->Value = NewTransform;
		PropagateTransformUpdate(Query, Entity);
	}
}

void FlecsTransformUtils::SetGlobalLocation(const TransformQuery& Query, const flecs::entity& Entity, const FVector& NewLocation)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalLocation(Query, Entity, *global_transform, NewLocation);
	}
}

void FlecsTransformUtils::SetGlobalLocation(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FVector& NewLocation)
{
	Transform.Value.SetTranslation(NewLocation);
	PropagateTransformUpdate(Query, Entity);
}

void FlecsTransformUtils::SetGlobalRotation(const TransformQuery& Query, const flecs::entity& Entity, const FQuat& NewRotation)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalRotation(Query, Entity, *global_transform, NewRotation);
	}
}

void FlecsTransformUtils::SetGlobalRotation(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FQuat& NewRotation)
{
	Transform.Value.SetRotation(NewRotation);
	PropagateTransformUpdate(Query, Entity);
}

void FlecsTransformUtils::SetGlobalTransform(const TransformQuery& Query, const flecs::entity& Entity, const FTransform& NewTransform)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalTransform(Query, Entity, *global_transform, NewTransform);
	}
}

void FlecsTransformUtils::SetGlobalTransform(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FTransform& NewTransform)
{
	Transform.Value = NewTransform;
	PropagateTransformUpdate(Query, Entity);
}

void FlecsTransformUtils::SetLocationAndRotation(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform,
	const FVector& NewLocation, const FQuat& NewRotation)
{
	Transform.Value.SetTranslation(NewLocation);
	Transform.Value.SetRotation(NewRotation);
	PropagateTransformUpdate(Query, Entity);
}

void FlecsTransformUtils::AddGlobalOffset(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FVector& Offset)
{
	Transform.Value.AddToTranslation(Offset);
	PropagateTransformUpdate(Query, Entity);
}

void FlecsTransformUtils::AttachEntityTo(const flecs::entity& Entity, const flecs::entity& Parent, const FTransform& RelativeTransform)
{
	Entity.set(FFlecsTransform(RelativeTransform)).set<FFlecsAttachedTo>(Parent, {});
}

void FlecsTransformUtils::UpdateLocalTransform(const FFlecsTransform& parent_transform, const FFlecsTransform& self_transform, FFlecsAttachedTo& local_transform)
{
	local_transform.Value = self_transform.Value.GetRelativeTransform(parent_transform.Value);
}

void FlecsTransformUtils::RegisterGlobalTransformSystem(const flecs::world& FlecsWorld)
{
	// - local transform
   //  - global parent transform (* = optional, so we match root entities)
   //  - global transform
	FlecsWorld.system<const FFlecsTransform, const FFlecsGlobalTransform*, FFlecsGlobalTransform>("SynchronizeTransforms")
	.term_at(1).cascade<FFlecsAttachedTo>()
	.multi_threaded().kind(flecs::OnValidate)
	.each([](const FFlecsTransform& local_transform, const FFlecsGlobalTransform* parent_global,
		FFlecsGlobalTransform& global_transform)
	{
		global_transform.Value = parent_global ? local_transform.Value * parent_global->Value : local_transform.Value;	
	});
}