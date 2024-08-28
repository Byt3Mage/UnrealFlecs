#pragma once

#include "flecs.h"
#include "JsonObjectConverter.h"
#include "StructView.h"
#include "Templates/Function.h"

struct UNREALFLECS_API FFlecsTypeRegistry
{
	using RegisterFn = TFunctionRef<void (const flecs::world&)>;
	using SetFn = TFunctionRef<void (flecs::entity&, const FConstStructView)>;
	using AddFn = TFunctionRef<void (flecs::entity&)>;
	using IDFn = TFunctionRef<flecs::id (const flecs::world&)>;
	
	static FFlecsTypeRegistry& Get()
	{
		static FFlecsTypeRegistry Instance;
		return Instance;
	}

	const TArray<RegisterFn>& GetRegisterFns() const { return RegisterFns; }
	
	void AddRegisterFn(RegisterFn Func)
	{
		RegisterFns.Add(Func);
	}
	
	void AddEntitySetFn(const UScriptStruct* Type, SetFn Func)
	{
		EntitySetFns.Add(Type, Func);
	}

	void AddEntityAddFn(const UScriptStruct* Type, AddFn Func)
	{
		EntityAddFns.Add(Type, Func);
	}

	void AddComponentID(const UScriptStruct* Type, IDFn Func)
	{
		ComponentIDs.Add(Type, Func);
	}

	SetFn* FindSetFn(const UScriptStruct* Type)
	{
		return EntitySetFns.Find(Type);
	}

	AddFn* FindAddFn(const UScriptStruct* Type)
	{
		return EntityAddFns.Find(Type);
	}

	flecs::id FindID(const UScriptStruct* Type, const flecs::world& FlecsWorld)
	{
		if (IDFn* Func = ComponentIDs.Find(Type))
		{
			return (*Func)(FlecsWorld);
		}

		return flecs::id();
	}
	
	// Use sparingly might be costly on performance
	const UScriptStruct* GetScriptStruct(const flecs::world& FlecsWorld, const flecs::id id) const
	{
		for (auto& [Struct, Fn] : ComponentIDs)
		{
			if (Fn(FlecsWorld) == id)
			{
				return Struct;
			}
		}

		return nullptr;
	}

private:
	TArray<RegisterFn> RegisterFns;
	TMap<const UScriptStruct*, SetFn> EntitySetFns;
	TMap<const UScriptStruct*, AddFn> EntityAddFns;
	TMap<const UScriptStruct*, IDFn> ComponentIDs;
};

template<typename T>
struct FFlecsJsonSerializer
{
	static int UStructSerializer(const flecs::serializer* s, const T* Data)
	{
		FString Json;
		FJsonObjectConverter::UStructToJsonObjectString<T>(*Data, Json);
		
		auto Src = StringCast<ANSICHAR>(*Json);
		const char* Ptr = Src.Get();
		return s->value(flecs::String, &Ptr);
	}
	
	static void UStructDeserializer(T* Dst, const char* value)
	{
		FJsonObjectConverter::JsonObjectStringToUStruct<T>(value, Dst);
	}
};

template <typename T>
struct FRegisterFlecsComponent
{
	static const FString Name;
	static const bool IsRegistered;
	static const bool UseDefaultSerializer;
	
	static bool Init()
	{
		if (!IsRegistered)
		{
			FFlecsTypeRegistry::Get().AddRegisterFn(&RegisterComponent);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Flecs component %s is already registered. Can not register component twice"), *Name);
		}
		
		return true;
	}

private:
	static void RegisterComponent(const flecs::world& FlecsWorld)
	{
		if (UseDefaultSerializer)
		{
			FlecsWorld.component<T>()
			.opaque(flecs::String)
			.serialize(&FFlecsJsonSerializer<T>::UStructSerializer)
			.assign_string(&FFlecsJsonSerializer<T>::UStructDeserializer);
		}

		if (const UScriptStruct* Struct = TBaseStructure<T>::Get())
		{
			FFlecsTypeRegistry& Registry = FFlecsTypeRegistry::Get();
			
			Registry.AddComponentID(Struct, &GetFlecsID);
			Registry.AddEntitySetFn(Struct, &SetOnEntity);
			Registry.AddEntityAddFn(Struct, &AddToEntity);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Flecs component %s registered"), *Name);
	}
	
	static void SetOnEntity(flecs::entity& E, const FConstStructView View)
	{
		if (const T* Data = View.GetPtr<const T>())
		{
			E.set<T>(*Data);
		}
	}

	static void AddToEntity(flecs::entity& E)
	{
		E.add<T>();
	}

	static flecs::id GetFlecsID(const flecs::world& FlecsWorld)
	{
		return FlecsWorld.id<T>();
	}
};
 
/*
 * Performs a plain component registration and stores its UScriptStruct type information.
 * If set, implements the unreal engine Json serializer for the component.
 * Does NOT add any flecs traits to the component.
*/
#define REG_FLECS_COMPONENT_IMPL(Type, bUseDefaultSerializer) \
struct Type; \
const FString FRegisterFlecsComponent<Type>::Name = FString(#Type); \
const bool FRegisterFlecsComponent<Type>::UseDefaultSerializer = (bUseDefaultSerializer); \
const bool FRegisterFlecsComponent<Type>::IsRegistered = FRegisterFlecsComponent<Type>::Init();

/*
 * Performs a plain component registration and stores its UScriptStruct type information.
 * Implements the unreal engine Json serializer for the component.
 * Does NOT add any flecs traits to the component.
*/
#define REG_FLECS_COMPONENT(Type) REG_FLECS_COMPONENT_IMPL(Type, true)