#include "FlecsCoreComponents.h"
#include "FlecsUtils.h"

void FlecsTransformUtils::PropagateTransformUpdates(const TransformQuery& Query, const flecs::entity& Parent)
{
	Query.iter(Parent)
	.set_group(Parent)
	.each([&Query](const flecs::entity child, const FFlecsTransform& parent_transform,
		const FFlecsAttachedTo& attachment, FFlecsTransform& child_transform)
	{
		child_transform.Value = attachment.Value * parent_transform.Value;
		PropagateTransformUpdates(Query, child);
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
			PropagateTransformUpdates(Query, Entity);
		}
	}
	// No local transform found, set global if available
	else if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		global_transform->Value = NewTransform;
		PropagateTransformUpdates(Query, Entity);
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
	PropagateTransformUpdates(Query, Entity);
	UpdateLocalTransform(Entity, Transform.Value);
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
	PropagateTransformUpdates(Query, Entity);
	UpdateLocalTransform(Entity, Transform.Value);
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
	PropagateTransformUpdates(Query, Entity);
	UpdateLocalTransform(Entity, Transform.Value);
}

void FlecsTransformUtils::SetLocationAndRotation(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform,
	const FVector& NewLocation, const FQuat& NewRotation)
{
	Transform.Value.SetTranslation(NewLocation);
	Transform.Value.SetRotation(NewRotation);
	
	PropagateTransformUpdates(Query, Entity);
	UpdateLocalTransform(Entity, Transform.Value);
}

void FlecsTransformUtils::AddGlobalOffset(const TransformQuery& Query, const flecs::entity& Entity, FFlecsTransform& Transform, const FVector& Offset)
{
	Transform.Value.AddToTranslation(Offset);
	PropagateTransformUpdates(Query, Entity);
	UpdateLocalTransform(Entity, Transform.Value);
}

void FlecsTransformUtils::AttachEntityTo(const TransformQuery& Query, const flecs::entity& Entity, const flecs::entity& Parent,
	const FTransform& RelativeTransform)
{
	auto* child_transform = Entity.get_mut<FFlecsTransform>();
	auto* parent_transform = Parent.get<FFlecsTransform>();
	
	if (child_transform && parent_transform)
	{
		FFlecsAttachedTo& attachment = flecs_utils::ensure_component<FFlecsAttachedTo>(Entity, Parent);
		attachment.Value = RelativeTransform;
		child_transform->Value = RelativeTransform * parent_transform->Value;
		PropagateTransformUpdates(Query, Entity);
	}
}

void FlecsTransformUtils::UpdateLocalTransform(const flecs::entity& Entity, const FTransform& Transform)
{
	auto[parent, attachment] = flecs_utils::get_pair_mut<FFlecsAttachedTo>(Entity);
	
	if (attachment)
	{
		// TODO: Consider handling missing parent transform
		auto* parent_transform = parent.get<FFlecsTransform>();
		attachment->Value = Transform.GetRelativeTransform(parent_transform->Value);
	}
}