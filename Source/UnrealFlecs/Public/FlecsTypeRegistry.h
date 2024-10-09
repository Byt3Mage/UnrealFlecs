#pragma once

#include "flecs.h"
#include "JsonObjectConverter.h"
#include "StructView.h"
#include "Templates/Function.h"

struct UNREALFLECS_API FFlecsTypeRegistry
{
	using RegisterFn = TFunction<void (const flecs::world&)>;
	using SetFn = TFunction<void (flecs::entity&, const FConstStructView)>;
	using AddFn = TFunction<void (flecs::entity&)>;
	using GetFn = TFunction<FConstStructView (flecs::entity&)>;
	using IDFn = TFunction<flecs::id (const flecs::world&)>;

	FFlecsTypeRegistry() = default;
	
	FFlecsTypeRegistry(const FFlecsTypeRegistry&) = delete;
	FFlecsTypeRegistry(FFlecsTypeRegistry&&) = delete;

	FFlecsTypeRegistry& operator=(const FFlecsTypeRegistry&) = delete;
	FFlecsTypeRegistry& operator=(FFlecsTypeRegistry&&) = delete;
	
	static FFlecsTypeRegistry& Get()
	{
		static FFlecsTypeRegistry Instance;
		return Instance;
	}

	const TArray<RegisterFn>& get_register_fns() const { return m_register_fns; }
	
	void add_register_fn(RegisterFn Func)
	{
		m_register_fns.Emplace(Func);
	}
	
	void add_entity_set_fn(const UScriptStruct* Type, SetFn Func)
	{
		m_entity_set_fns.Emplace(Type, Func);
	}

	void add_entity_add_fn(const UScriptStruct* Type, AddFn Func)
	{
		m_entity_add_fns.Emplace(Type, Func);
	}

	void add_entity_get_fn(const UScriptStruct* Type, GetFn Func)
	{
		m_entity_get_fns.Emplace(Type, Func);
	}

	void add_component_id(const UScriptStruct* Type, IDFn Func)
	{
		m_component_ids.Emplace(Type, Func);
	}

	const SetFn* find_set_fn(const UScriptStruct* Type)
	{
		return m_entity_set_fns.Find(Type);
	}

	const AddFn* find_add_fn(const UScriptStruct* Type)
	{
		return m_entity_add_fns.Find(Type);
	}

	const GetFn* find_get_fn(const UScriptStruct* Type)
	{
		return m_entity_get_fns.Find(Type);
	}

	flecs::id find_id(const UScriptStruct* Type, const flecs::world& FlecsWorld)
	{
		if (IDFn* Func = m_component_ids.Find(Type))
		{
			return (*Func)(FlecsWorld);
		}

		return flecs::id();
	}
	
	// Use sparingly might be costly on performance
	const UScriptStruct* get_script_struct(const flecs::world& FlecsWorld, const flecs::id id) const
	{
		for (auto& [Struct, Fn] : m_component_ids)
		{
			if (Fn(FlecsWorld) == id)
			{
				return Struct;
			}
		}

		return nullptr;
	}

private:
	TArray<RegisterFn> m_register_fns;
	TMap<const UScriptStruct*, SetFn> m_entity_set_fns;
	TMap<const UScriptStruct*, AddFn> m_entity_add_fns;
	TMap<const UScriptStruct*, GetFn> m_entity_get_fns;
	TMap<const UScriptStruct*, IDFn> m_component_ids;
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
			FFlecsTypeRegistry::Get().add_register_fn(&RegisterComponent);
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
		auto component = FlecsWorld.component<T>();
		
		if (UseDefaultSerializer)
		{
			component.opaque(flecs::String)
			.serialize(&FFlecsJsonSerializer<T>::UStructSerializer)
			.assign_string(&FFlecsJsonSerializer<T>::UStructDeserializer);
		}

		if (const UScriptStruct* Struct = TBaseStructure<T>::Get())
		{
			FFlecsTypeRegistry& Registry = FFlecsTypeRegistry::Get();
			
			Registry.add_component_id(Struct, &get_flecs_id);
			Registry.add_entity_set_fn(Struct, &set_on_entity);
			Registry.add_entity_add_fn(Struct, &add_to_entity);
			Registry.add_entity_get_fn(Struct, &get_from_entity);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Flecs component %s registered"), *Name);
	}
	
	static void set_on_entity(flecs::entity& E, const FConstStructView View)
	{
		if (const T* Data = View.GetPtr<const T>())
		{
			E.set<T>(*Data);
		}
	}

	static void add_to_entity(flecs::entity& E)
	{
		E.add<T>();
	}

	static FConstStructView get_from_entity(const flecs::entity& E)
	{
		if (const T* component = E.get<T>())
		{
			return FConstStructView(TBaseStructure<T>::Get(), reinterpret_cast<const uint8*>(component));
		}

		return FConstStructView();
	}

	static flecs::id get_flecs_id(const flecs::world& FlecsWorld)
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