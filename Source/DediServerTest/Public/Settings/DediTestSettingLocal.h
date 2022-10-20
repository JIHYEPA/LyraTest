// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Input/DediTestMappableConfigPair.h"
#include "DediTestSettingLocal.generated.h"

/**
 * 
 */
UCLASS()
class DEDISERVERTEST_API UDediTestSettingLocal : public UGameUserSettings
{
	GENERATED_BODY()


public:

	UDediTestSettingLocal();

	static UDediTestSettingLocal* Get();

	//~UGameUserSettings interface
	//virtual void ApplyNonResolutionSettings() override;
	//~End of UGameUserSettings interface

	//void OnExperienceLoaded();

public:

	DECLARE_EVENT_OneParam(UDediTestSettingLocal, FInputConfigDelegate, const FLoadedMappableConfigPair& /*Config*/);

	/** Delegate called when a new input config has been registered */
	FInputConfigDelegate OnInputConfigRegistered;

	/** Delegate called when a registered input config has been activated */
	FInputConfigDelegate OnInputConfigActivated;

	/** Delegate called when a registered input config has been deactivate */
	FInputConfigDelegate OnInputConfigDeactivated;

	/** Register the given input config with the settings to make it available to the player. */
	void RegisterInputConfig(ECommonInputType Type, const UPlayerMappableInputConfig* NewConfig, const bool bIsActive);

	/** Unregister the given input config. Returns the number of configs removed. */
	int32 UnregisterInputConfig(const UPlayerMappableInputConfig* ConfigToRemove);

	/** Set a registered input config as active */
	void ActivateInputConfig(const UPlayerMappableInputConfig* Config);

	/** Deactivate a registered config */
	void DeactivateInputConfig(const UPlayerMappableInputConfig* Config);

	/** Get all currently registered input configs */
	const TArray<FLoadedMappableConfigPair>& GetAllRegisteredInputConfigs() const { return RegisteredInputConfigs; }

private:

	UPROPERTY(VisibleAnywhere)
	TArray<FLoadedMappableConfigPair> RegisteredInputConfigs;

private:

	//void ReapplyThingsDueToPossibleDeviceProfileChange();
};
