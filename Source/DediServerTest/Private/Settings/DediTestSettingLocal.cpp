// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/DediTestSettingLocal.h"

UDediTestSettingLocal::UDediTestSettingLocal()
{
}

UDediTestSettingLocal* UDediTestSettingLocal::Get()
{
	return GEngine ? CastChecked<UDediTestSettingLocal>(GEngine->GetGameUserSettings()) : nullptr;
}

//void UDediTestSettingLocal::ApplyNonResolutionSettings()
//{
//	Super::ApplyNonResolutionSettings();
//
//	//...
//}
//
//void UDediTestSettingLocal::OnExperienceLoaded()
//{
//	ReapplyThingsDueToPossibleDeviceProfileChange();
//}
//
//void UDediTestSettingLocal::ReapplyThingsDueToPossibleDeviceProfileChange()
//{
//	ApplyNonResolutionSettings();
//}

void UDediTestSettingLocal::RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig, const bool bIsActive)
{
	if (NewConfig)
	{
		const int32 ExistingConfigIdx = RegisteredInputConfigs.IndexOfByPredicate([&NewConfig](const FLoadedMappableConfigPair& Pair) { return Pair.Config == NewConfig; });
		if (ExistingConfigIdx == INDEX_NONE)
		{
			const int32 NumAdded = RegisteredInputConfigs.Add(FLoadedMappableConfigPair(NewConfig, Type, bIsActive));
			if (NumAdded != INDEX_NONE)
			{
				OnInputConfigRegistered.Broadcast(RegisteredInputConfigs[NumAdded]);
			}
		}
	}
}

int32 UDediTestSettingLocal::UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove)
{
	if (ConfigToRemove)
	{
		const int32 Index = RegisteredInputConfigs.IndexOfByPredicate([&ConfigToRemove](const FLoadedMappableConfigPair& Pair) { return Pair.Config == ConfigToRemove; });
		if (Index != INDEX_NONE)
		{
			RegisteredInputConfigs.RemoveAt(Index);
			return 1;
		}

	}
	return INDEX_NONE;
}

void UDediTestSettingLocal::ActivateInputConfig(const UPlayerMappableInputConfig* Config)
{
	if (Config)
	{
		const int32 ExistingConfigIdx = RegisteredInputConfigs.IndexOfByPredicate([&Config](const FLoadedMappableConfigPair& Pair) { return Pair.Config == Config; });
		if (ExistingConfigIdx != INDEX_NONE)
		{
			RegisteredInputConfigs[ExistingConfigIdx].bIsActive = true;
			OnInputConfigActivated.Broadcast(RegisteredInputConfigs[ExistingConfigIdx]);
		}
	}
}

void UDediTestSettingLocal::DeactivateInputConfig(const UPlayerMappableInputConfig* Config)
{
	if (Config)
	{
		const int32 ExistingConfigIdx = RegisteredInputConfigs.IndexOfByPredicate([&Config](const FLoadedMappableConfigPair& Pair) { return Pair.Config == Config; });
		if (ExistingConfigIdx != INDEX_NONE)
		{
			RegisteredInputConfigs[ExistingConfigIdx].bIsActive = false;
			OnInputConfigDeactivated.Broadcast(RegisteredInputConfigs[ExistingConfigIdx]);
		}
	}
}

