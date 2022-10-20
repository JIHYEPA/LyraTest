// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/DediServerTestPawnComponent.h"
#include "GameplayTagContainer.h"
#include "Input/DediTestMappableConfigPair.h"
#include "DediServerTestHeroComponent.generated.h"

struct FInputActionValue;

/**
 * 
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class DEDISERVERTEST_API UDediServerTestHeroComponent : public UDediServerTestPawnComponent
{
	GENERATED_BODY()

public:

	UDediServerTestHeroComponent(const FObjectInitializer& ObjectInitializer);

	// 특정 Actor에 존재하는 DediServerTest Hero Component를 반환.
	UFUNCTION(BlueprintPure, Category = "DediServerTest|Pawn")
	static UDediServerTestHeroComponent* FindHeroComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDediServerTestHeroComponent>() : nullptr); }

	bool HasPawnInitialized() const;

	bool IsReadyToBindInputs() const;

protected:

	virtual void OnRegister() override;

	virtual bool IsPawnComponentReadyToInitialize() const override;
	void OnPawnReadyToInitialize();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);

	void Input_Look(const FInputActionValue& InputActionValue);

	void OnInputConfigActivated(const FLoadedMappableConfigPair& ConfigPair);
	void OnInputConfigDeactivated(const FLoadedMappableConfigPair& ConfigPair);

protected:

	bool bPawnHasInitialized;

	bool bReadyToBindInputs;
};
