// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DediTestGameState.generated.h"

class UDediExperienceManagerComponent;

/**
 * 
 */
UCLASS(Config = Game)
class DEDISERVERTEST_API ADediTestGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ADediTestGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:

	UPROPERTY()
	UDediExperienceManagerComponent* ExperienceManagerComponent;
};
