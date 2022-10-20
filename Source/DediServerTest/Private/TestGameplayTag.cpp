// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGameplayTag.h"
#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"

FTestGameplayTags FTestGameplayTags::GameplayTag;

void FTestGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagManager = UGameplayTagsManager::Get();

	GameplayTag.AddAllTags(GameplayTagManager);

	GameplayTagManager.DoneAddingNativeTags();
}

void FTestGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_Move, "InputTag.Move", "Move input.");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (Mouse) input");
	AddTag(InputTag_Jump, "InputTag.Jump", "Jump input.");
}

void FTestGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
