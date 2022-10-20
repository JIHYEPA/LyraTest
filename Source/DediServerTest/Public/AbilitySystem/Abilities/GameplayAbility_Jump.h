// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DediServerTestGameplayAbility.h"
#include "GameplayAbility_Jump.generated.h"

/**
 * 
 */
UCLASS()
class DEDISERVERTEST_API UGameplayAbility_Jump : public UDediServerTestGameplayAbility
{
	GENERATED_BODY()

public:

	UGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Ability")
	void CharacterJumpStop();
};
