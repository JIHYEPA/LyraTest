// Copyright Epic Games, Inc. All Rights Reserved.

#include "DediServerTest.h"
#include "TestGameplayTag.h"
#include "GameplayTagsManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDediServerTestGameModule, DediServerTest, "DediServerTest" );

void FDediServerTestGameModule::StartupModule()
{
	FTestGameplayTags::InitializeNativeTags();

	UGameplayTagsManager::Get().DoneAddingNativeTags();
}