// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/DediTestGameState.h"
#include "Online/DediExperienceManagerComponent.h"

ADediTestGameState::ADediTestGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	ExperienceManagerComponent = CreateDefaultSubobject<UDediExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
