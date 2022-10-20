// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "DediTestWorldSettings.generated.h"

class UDediTestExperienceDefinition;

/**
 * 
 */
UCLASS()
class DEDISERVERTEST_API ADediTestWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	ADediTestWorldSettings(const FObjectInitializer& ObjectInitializer);

	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:

	// The default experience to use when a server opens this map if it is not overridden by the user-facing experience
	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	TSoftClassPtr<UDediTestExperienceDefinition> DefaultGameplayExperience;
};
