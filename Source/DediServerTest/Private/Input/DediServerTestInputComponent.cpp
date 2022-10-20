// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/DediServerTestInputComponent.h"
#include "Settings/DediTestSettingLocal.h"

UDediServerTestInputComponent::UDediServerTestInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UDediServerTestInputComponent::AddInputMappings(const UTestInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	/*ULyraLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULyraLocalPlayer>();
	check(LocalPlayer);*/

	// Add any registered input mappings from the settings!
	if (UDediTestSettingLocal* LocalSettings = UDediTestSettingLocal::Get())
	{
		// We don't want to ignore keys that were "Down" when we add the mapping context
		// This allows you to die holding a movement key, keep holding while waiting for respawn,
		// and have it be applied after you respawn immediately. Leaving bIgnoreAllPressedKeysUntilRelease
		// to it's default "true" state would require the player to release the movement key,
		// and press it again when they respawn
		FModifyContextOptions Options = {};
		Options.bIgnoreAllPressedKeysUntilRelease = false;

		// Add all registered configs, which will add every input mapping context that is in it
		const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : Configs)
		{
			if (Pair.bIsActive)
			{
				InputSubsystem->AddPlayerMappableConfig(Pair.Config, Options);
			}
		}

		// Tell enhanced input about any custom keymappings that we have set 
		/*for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			if (Pair.Key != NAME_None && Pair.Value.IsValid())
			{
				InputSubsystem->AddPlayerMappedKey(Pair.Key, Pair.Value);
			}
		}*/
	}
}

void UDediServerTestInputComponent::RemoveInputMappings(const UTestInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	/*ULyraLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULyraLocalPlayer>();
	check(LocalPlayer);*/

	if (UDediTestSettingLocal* LocalSettings = UDediTestSettingLocal::Get())
	{
		// Remove any registered input contexts
		const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : Configs)
		{
			InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
		}

		// Clear any player mapped keys from enhanced input
		/*for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			InputSubsystem->RemovePlayerMappedKey(Pair.Key);
		}*/
	}
}

void UDediServerTestInputComponent::AddInputConfig(const FLoadedMappableConfigPair& ConfigPair,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	check(InputSubsystem);
	if (ensure(ConfigPair.bIsActive))
	{
		InputSubsystem->AddPlayerMappableConfig(ConfigPair.Config);
	}
}

void UDediServerTestInputComponent::RemoveInputConfig(const FLoadedMappableConfigPair& ConfigPair,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem)
{
	check(InputSubsystem);
	if (!ConfigPair.bIsActive)
	{
		InputSubsystem->AddPlayerMappableConfig(ConfigPair.Config);
	}
}
