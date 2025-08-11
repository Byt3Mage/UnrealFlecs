// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealFlecs.h"

#include "FlecsTypeRegistry.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FUnrealFlecsModule"

DEFINE_LOG_CATEGORY(LogUnrealFlecs);

void FUnrealFlecsModule::StartupModule()
{
	FCoreDelegates::OnAllModuleLoadingPhasesComplete.AddLambda([]()
	{
		FFlecsTypeRegistry::Get().FlushRegistrationQueue();
	});
}

void FUnrealFlecsModule::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealFlecsModule, UnrealFlecs)
