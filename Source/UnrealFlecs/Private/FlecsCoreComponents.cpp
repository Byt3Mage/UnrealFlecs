#include "FlecsCoreComponents.h"

#include "FlecsWorldContext.h"

void FlecsTransformUtils::PropagateTransformUpdates(TransformQuery& AttachmentQuery, const flecs::entity& Parent)
{
	AttachmentQuery.set_group(Parent)
	.each([&AttachmentQuery](const flecs::entity child, const FFlecsTransform& parent_transform,
		const FFlecsAttachedTo& local_transform, FFlecsTransform& child_transform)
	{
		child_transform.Value = local_transform.Value * parent_transform.Value;
		PropagateTransformUpdates(AttachmentQuery, child);
	});
}

void FlecsTransformUtils::SetLocalTransform(const flecs::entity& Entity, const FTransform& NewTransform)
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
			auto* flecs_context = static_cast<FlecsWorldContext*>(Entity.world().get_ctx());
			global_transform->Value = NewTransform * parent_transform->Value;
			PropagateTransformUpdates(flecs_context->TransformQuery, Entity);
		}
	}
	// No local transform found set global instead
	else if (auto* global_transform = Entity.get_mut<FFlecsAttachedTo>())
	{
		auto* flecs_context = static_cast<FlecsWorldContext*>(Entity.world().get_ctx());
		global_transform->Value = NewTransform;
		PropagateTransformUpdates(flecs_context->TransformQuery, Entity);
	}
}

void FlecsTransformUtils::SetGlobalLocation(const flecs::entity& Entity, const FVector& NewLocation)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalLocation(Entity, *global_transform, NewLocation);
	}
}

void FlecsTransformUtils::SetGlobalLocation(const flecs::entity& Entity, Global& Transform, const FVector& NewLocation)
{
	auto* flecs_context = static_cast<FlecsWorldContext*>(Entity.world().get_ctx());
	Transform.Value.SetTranslation(NewLocation);
	PropagateTransformUpdates(flecs_context->TransformQuery, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}

void FlecsTransformUtils::SetGlobalRotation(const flecs::entity& Entity, const FQuat& NewRotation)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalRotation(Entity, *global_transform, NewRotation);
	}
}

void FlecsTransformUtils::SetGlobalRotation(const flecs::entity& Entity, Global& Transform, const FQuat& NewRotation)
{
	auto* flecs_context = static_cast<FlecsWorldContext*>(Entity.world().get_ctx());
	Transform.Value.SetRotation(NewRotation);
	PropagateTransformUpdates(flecs_context->TransformQuery, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}

void FlecsTransformUtils::SetGlobalTransform(const flecs::entity& Entity, const FTransform& NewTransform)
{
	if (auto* global_transform = Entity.get_mut<FFlecsTransform>())
	{
		SetGlobalTransform(Entity, *global_transform, NewTransform);
	}
}

void FlecsTransformUtils::SetGlobalTransform(const flecs::entity& Entity, Global& Transform, const FTransform& NewTransform)
{
	auto* flecs_context = static_cast<FlecsWorldContext*>(Entity.world().get_ctx());
	Transform.Value = NewTransform;
	PropagateTransformUpdates(flecs_context->TransformQuery, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}

void FlecsTransformUtils::SetGlobalLocationAndRotation(const flecs::entity& Entity, Global& Transform,
	const FVector& NewLocation, const FQuat& NewRotation)
{
	auto* flecs_context = static_cast<FlecsWorldContext*>(Entity.world().get_ctx());
	Transform.Value.SetTranslation(NewLocation);
	Transform.Value.SetRotation(NewRotation);
	PropagateTransformUpdates(flecs_context->TransformQuery, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}

void FlecsTransformUtils::AddGlobalOffset(const flecs::entity& Entity, Global& Transform, const FVector& Offset)
{
	auto* flecs_context = static_cast<FlecsWorldContext*>(Entity.world().get_ctx());
	Transform.Value.AddToTranslation(Offset);
	PropagateTransformUpdates(flecs_context->TransformQuery, Entity);

	// update local transform
	if (flecs::entity Parent = Entity.target<FFlecsAttachedTo>())
	{
		auto* attachment = Entity.get_mut<FFlecsAttachedTo>(Parent);
		auto* parent_transform = Parent.get<FFlecsTransform>();
		attachment->Value = Transform.Value.GetRelativeTransform(parent_transform->Value);
	}
}