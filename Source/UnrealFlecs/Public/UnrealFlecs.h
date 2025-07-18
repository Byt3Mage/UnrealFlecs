// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "flecs.h"
#include "unordered_dense.h"

class FUnrealFlecsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

namespace UnrealFlecs
{
	template<typename K, typename V>
	using HashMap = ankerl::unordered_dense::map<K, V>;

	template<typename K>
	using HashSet = ankerl::unordered_dense::set<K>;
}

template<>
struct ankerl::unordered_dense::hash<flecs::entity>
{
	using is_avalanching = void;
	
	auto operator()(flecs::entity const& e) const noexcept -> uint64_t
	{
		return detail::wyhash::hash(e);
	}
};
