// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DediServerTestPlayerController.h"
#include "AbilitySystem/DediTestAbilitySystemComponent.h"
#include "Player/DediServerTestPlayerState.h"
#include "Actor/DediServerTestCharacter.h"

ADediServerTestPlayerController::ADediServerTestPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

ADediServerTestPlayerState* ADediServerTestPlayerController::GetDediServerTestPlayerState() const
{
	return CastChecked<ADediServerTestPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UDediTestAbilitySystemComponent* ADediServerTestPlayerController::GetDediTestAbilitySystemComponent() const
{
	const ADediServerTestPlayerState* DediServerTestPS = GetDediServerTestPlayerState();
	return (DediServerTestPS ? DediServerTestPS->GetDediTestAbilitySystemComponent() : nullptr);
}

void ADediServerTestPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ADediServerTestPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UDediTestAbilitySystemComponent* DediTestASC = GetDediTestAbilitySystemComponent())
	{
		DediTestASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
