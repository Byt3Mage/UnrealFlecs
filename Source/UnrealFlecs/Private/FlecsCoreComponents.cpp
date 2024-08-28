#include "FlecsCoreComponents.h"

void FlecsTransformUtils::PropagateTransformUpdates(TransformQuery& Query, const flecs::entity& Parent)
{
	Query.set_group(Parent)
	.each([&Query](const flecs::entity child, const FFlecsTransform& parent_transform,
		const FFlecsAttachedTo& local_transform, FFlecsTransform& child_transform)
	{
		child_transform.Value = local_transform.Value * parent_transform.Value;
		PropagateTransformUpdates(Query, child);
	});
}

void FlecsTransformUtils::SetLocalTransform(TransformQuery& Query, const flecs::entity& Entity, const FTransform& NewTransform)
{
	flecs::entity Parent = Entity.target<FFlecsAttachedTo>();
	
	auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
	auto* parent_transform = Parent.get<FFlecsTransform>();
	
	if (attachment && parent_transform)
	{
		attachment->Value = NewTransform;

		// Update global transform
		if (auto* global_transform = Entity.get_mut<FFlecsAttachedTo>())
		{
			global_transform->Value = NewTransform * parent_transform->Value;
			PropagateTransformUpdates(Query, Entity);
		}
	}
	// No local transform found, set global instead
	else if (auto* global_transform = Entity.get_mut<FFlecsAttachedTo>())
	{
		global_transform->Value = NewTransform;
		PropagateTransformUpdates(Query, Entity);
	}
}

void FlecsTransformUtils::SetGlobalLocation(TransformQuery& Query, const flecs::entity& Entity, const FVector& NewLocation)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalLocation(Query, Entity, *global_transform, NewLocation);
	}
}

void FlecsTransformUtils::SetGlobalLocation(TransformQuery& Query, const flecs::entity& Entity, Global& Transform, const FVector& NewLocation)
{
	Transform.Value.SetTranslation(NewLocation);
	PropagateTransformUpdates(Query, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}

void FlecsTransformUtils::SetGlobalRotation(TransformQuery& Query, const flecs::entity& Entity, const FQuat& NewRotation)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalRotation(Query, Entity, *global_transform, NewRotation);
	}
}

void FlecsTransformUtils::SetGlobalRotation(TransformQuery& Query, const flecs::entity& Entity, Global& Transform, const FQuat& NewRotation)
{
	Transform.Value.SetRotation(NewRotation);
	PropagateTransformUpdates(Query, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}

void FlecsTransformUtils::SetGlobalTransform(TransformQuery& Query, const flecs::entity& Entity, const FTransform& NewTransform)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalTransform(Query, Entity, *global_transform, NewTransform);
	}
}

void FlecsTransformUtils::SetGlobalTransform(TransformQuery& Query, const flecs::entity& Entity, Global& Transform, const FTransform& NewTransform)
{
	Transform.Value = NewTransform;
	PropagateTransformUpdates(Query, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}

void FlecsTransformUtils::SetGlobalLocationAndRotation(TransformQuery& Query, const flecs::entity& Entity, Global& Transform,
	const FVector& NewLocation, const FQuat& NewRotation)
{
	Transform.Value.SetTranslation(NewLocation);
	Transform.Value.SetRotation(NewRotation);
	PropagateTransformUpdates(Query, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}

void FlecsTransformUtils::AddGlobalOffset(TransformQuery& Query, const flecs::entity& Entity, Global& Transform, const FVector& Offset)
{
	Transform.Value.AddToTranslation(Offset);
	PropagateTransformUpdates(Query, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}