// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DediTestExperienceDefinition.generated.h"

class UGameFeatureAction;

/**
 * 
 */
UCLASS()
class DEDISERVERTEST_API UDediTestExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UDediTestExperienceDefinition();

public:

	// List of Game Feature Plugins this experience wants to have active
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;
};
