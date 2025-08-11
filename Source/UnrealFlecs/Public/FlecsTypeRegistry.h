#pragma once

#include <ranges>

#include "flecs.h"
#include "JsonObjectConverter.h"
#include "UnrealFlecs.h"
#include "Logging/StructuredLog.h"
#include "StructUtils/StructView.h"
#include "Templates/Function.h"

struct UNREALFLECS_API FFlecsTypeRegistry
{
	using RegisterFn = void(*)(const flecs::world&);
	using SetFn = void(*)(const flecs::entity&, const FConstStructView);
	using AddFn = void(*)(const flecs::entity&);
	using GetFn = FConstStructView(*)(const flecs::entity&);
	using IDFn  = flecs::id(*)(const flecs::world&);
	using EnqueuedFn = void(*)(FFlecsTypeRegistry&);

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

	static void RegisterAllTypes(const flecs::world& FlecsWorld)
	{
		for (const auto& Fn : Get().GetRegisterFns() | std::views::values)
		{
			Fn(FlecsWorld);
		}
	}
	
	auto GetRegisterFns() const -> const auto&
	{
		return RegisterFuncs.values();
	}
	
	void InsertRegisterFn(const UScriptStruct* Type, RegisterFn Func)
	{
		RegisterFuncs.insert_or_assign(Type, Func);
	}
	
	void InsertSetFn(const UScriptStruct* Type, SetFn Func)
	{
		SetFuncs.insert_or_assign(Type, Func);
	}

	void InsertAddFn(const UScriptStruct* Type, AddFn Func)
	{
		AddFuncs.insert_or_assign(Type, Func);
	}

	void InsertGetFn(const UScriptStruct* Type, GetFn Func)
	{
		GetFuncs.insert_or_assign(Type, Func);
	}

	void InsertIdFn(const UScriptStruct* Type, IDFn Func)
	{
		Ids.insert_or_assign(Type, Func);
	}

	const SetFn* FindSetFn(const UScriptStruct* Type) const
	{
		const auto It = SetFuncs.find(Type);
		return It != SetFuncs.end() ? &It->second : nullptr;
	}

	const AddFn* FindAddFn(const UScriptStruct* Type) const
	{
		const auto It = AddFuncs.find(Type);
		return It != AddFuncs.end() ? &It->second : nullptr;
	}
 
	const GetFn* FindGetFn(const UScriptStruct* Type) const
	{
		const auto It = GetFuncs.find(Type);
		return It != GetFuncs.end() ? &It->second : nullptr;
	}

	flecs::id FindId(const UScriptStruct* Type, const flecs::world& FlecsWorld) const
	{
		if (const auto It = Ids.find(Type); It != Ids.end())
		{
			return (It->second)(FlecsWorld);
		}

		return flecs::id();
	}
	
	// Use sparingly, might be costly on performance
	const UScriptStruct* GetScriptStruct(const flecs::world& FlecsWorld, const flecs::id id) const
	{
		for (auto& [Struct, Fn] : Ids)
		{
			if (Fn(FlecsWorld) == id)
			{
				return Struct;
			}
		}

		return nullptr;
	}
	
	void EnqueueRegistration(EnqueuedFn Fn)
	{
		EnqueuedFuncs.insert(Fn);
	}

	void FlushRegistrationQueue()
	{
		for (const auto Fn : EnqueuedFuncs)
		{
			Fn(*this);
		}
		
		EnqueuedFuncs.clear();
	}

private:
	UFlecs::HashMap<const UScriptStruct*, RegisterFn> RegisterFuncs;
	UFlecs::HashMap<const UScriptStruct*, SetFn> SetFuncs;
	UFlecs::HashMap<const UScriptStruct*, AddFn> AddFuncs;
	UFlecs::HashMap<const UScriptStruct*, GetFn> GetFuncs;
	UFlecs::HashMap<const UScriptStruct*, IDFn> Ids;
	UFlecs::HashSet<EnqueuedFn> EnqueuedFuncs;
};

template<typename T>
struct FFlecsJsonSerializer
{
	static int UStructSerializer(const flecs::serializer* s, const T* Data)
	{
		FString Json;
		FJsonObjectConverter::UStructToJsonObjectString<T>(*Data, Json);

		const auto Src = StringCast<ANSICHAR>(*Json);
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
		FFlecsTypeRegistry::Get().EnqueueRegistration(AddToRegistry);
		return true;
	}

private:
	static void AddToRegistry(FFlecsTypeRegistry& Registry)
	{
		Registry.InsertRegisterFn(TBaseStructure<T>::Get(), &RegisterComponent);
		UE_LOGFMT(LogUnrealFlecs, Warning, "Flecs component {Comp} registered", ("Comp", Name));
	}
	
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
			
			Registry.InsertIdFn (Struct, &GetFlecsId);
			Registry.InsertSetFn(Struct, &SetOnEntity);
			Registry.InsertAddFn(Struct, &AddToEntity);
			Registry.InsertGetFn(Struct, &GetFromEntity);
		}
	}
	
	static void SetOnEntity(const flecs::entity& E, const FConstStructView View)
	{
		if (const T* Data = View.GetPtr<const T>())
		{
			E.set<T>(*Data);
		}
	}

	static void AddToEntity(const flecs::entity& E)
	{
		E.add<T>();
	}

	static FConstStructView GetFromEntity(const flecs::entity& E)
	{
		return FConstStructView::Make(E.get<T>());
	}

	static flecs::id GetFlecsId(const flecs::world& FlecsWorld)
	{
		return FlecsWorld.id<T>();
	}
};

/*
 * Performs a plain component registration and stores its UScriptStruct type information.
 * Implements the unreal engine Json serializer for the component.
 * Does NOT add any flecs traits to the component.
*/
#define REG_FLECS_COMPONENT(Type) \
struct Type; \
const FString FRegisterFlecsComponent<Type>::Name = FString(#Type); \
const bool FRegisterFlecsComponent<Type>::UseDefaultSerializer = (true); \
const bool FRegisterFlecsComponent<Type>::IsRegistered = FRegisterFlecsComponent<Type>::Init();