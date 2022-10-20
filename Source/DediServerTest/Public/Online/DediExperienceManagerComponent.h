// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "DediExperienceManagerComponent.generated.h"

class UDediTestExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDediTestExperienceLoaded, const UDediTestExperienceDefinition* /*Experience*/);

enum class EDediTestExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

/**
 * 
 */
UCLASS()
class DEDISERVERTEST_API UDediExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:

	UDediExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

#if WITH_SERVER_CODE
	void ServerSetCurrentExperience(FPrimaryAssetId ExperienceId);
#endif

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded(FOnDediTestExperienceLoaded::FDelegate&& Delegate);

	// Returns true if the experience is fully loaded
	bool IsExperienceLoaded() const;

private:

	UFUNCTION()
	void OnRep_CurrentExperience();

	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

private:

	UPROPERTY(ReplicatedUsing = OnRep_CurrentExperience)
	const UDediTestExperienceDefinition* CurrentExperience;

	EDediTestExperienceLoadState LoadState = EDediTestExperienceLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	/**
	 * Delegate called when the experience has finished loading just before others
	 * (e.g., subsystems that set up for regular gameplay)
	 */
	//FOnDediTestExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	FOnDediTestExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	//FOnDediTestExperienceLoaded OnExperienceLoaded_LowPriority;
};
