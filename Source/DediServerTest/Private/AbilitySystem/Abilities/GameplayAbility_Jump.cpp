// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_Jump.h"
#include "Actor/DediServerTestCharacter.h"

UGameplayAbility_Jump::UGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!ActorInfo || ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ADediServerTestCharacter* DediServerTestCharacter = Cast<ADediServerTestCharacter>(ActorInfo->AvatarActor.Get());
	if(!DediServerTestCharacter || !DediServerTestCharacter->CanJump())
	{
		return false;
	}

	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_Jump::CharacterJumpStart()
{
	if(ADediServerTestCharacter* DediServerTestCharacter = GetDediServerTestCharacterFromActorInfo())
	{
		if(DediServerTestCharacter->IsLocallyControlled() && !DediServerTestCharacter->bPressedJump)
		{
			DediServerTestCharacter->UnCrouch();
			DediServerTestCharacter->Jump();
		}
	}
}

void UGameplayAbility_Jump::CharacterJumpStop()
{
	if (ADediServerTestCharacter* DediServerTestCharacter = GetDediServerTestCharacterFromActorInfo())
	{
		if (DediServerTestCharacter->IsLocallyControlled() && DediServerTestCharacter->bPressedJump)
		{
			DediServerTestCharacter->StopJumping();
		}
	}
}
