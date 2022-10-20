// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/DediTestWorldSettings.h"
#include "DediServerTestLogChannels.h"
#include "Engine/AssetManager.h"
#include "Online/DediTestExperienceDefinition.h"

ADediTestWorldSettings::ADediTestWorldSettings(const FObjectInitializer& ObjectInitializer)
{
}

FPrimaryAssetId ADediTestWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameplayExperience.IsNull())
	{
		Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());

		if (!Result.IsValid())
		{
			UE_LOG(LogDediServerTest, Error, TEXT("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings"),
				*GetPathNameSafe(this), *DefaultGameplayExperience.ToString());
		}
	}
	return Result;
}
