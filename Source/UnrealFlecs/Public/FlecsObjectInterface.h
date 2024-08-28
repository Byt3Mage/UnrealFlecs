#pragma once

#include "FlecsEntityHandle.h"
#include "UObject/Interface.h"
#include "FlecsObjectInterface.generated.h"

UINTERFACE()
class UNREALFLECS_API UFlecsObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class UNREALFLECS_API IFlecsObjectInterface
{
	GENERATED_BODY()

public:
	virtual FFlecsEntityHandle GetFlecsEntityHandle() const = 0;

	virtual void SetFlecsEntityHandle(const flecs::entity& Entity) = 0;

	virtual UMeshComponent* GetPrimaryMesh() = 0;
};