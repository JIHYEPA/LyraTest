// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DediServerTestPlayerController.generated.h"

class ADediServerTestPlayerState;
class UDediTestAbilitySystemComponent;

/**
 * 
 */
UCLASS(Config = Game)
class DEDISERVERTEST_API ADediServerTestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ADediServerTestPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Lyra|PlayerController")
	ADediServerTestPlayerState* GetDediServerTestPlayerState() const;

	UFUNCTION(BlueprintPure, Category = "DediServerTest|PlayerController")
	UDediTestAbilitySystemComponent* GetDediTestAbilitySystemComponent() const;

protected:

	//~APlayerController interface
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface
};
