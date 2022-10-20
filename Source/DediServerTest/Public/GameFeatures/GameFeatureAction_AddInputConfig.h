// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "Input/DediTestMappableConfigPair.h"
#include "GameFeatureAction_AddInputConfig.generated.h"

/**
 * 
 */
UCLASS(meta=(DisplayName = "Add Input Config"))
class DEDISERVERTEST_API UGameFeatureAction_AddInputConfig : public UGameFeatureAction
{
	GENERATED_BODY()

public:

	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;

	UPROPERTY(EditAnywhere)
	TArray<FMappableConfigPair> InputConfigs;
};
