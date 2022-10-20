// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DediServerTestGameplayAbility.h"
#include "TestGameplayTag.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/DediTestAbilitySystemComponent.h"
#include "Actor/DediServerTestCharacter.h"

UDediServerTestGameplayAbility::UDediServerTestGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EAbilityActivationGroup::Independent;
}

ADediServerTestCharacter* UDediServerTestGameplayAbility::GetDediServerTestCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ADediServerTestCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

void UDediServerTestGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
                                                               const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);

	// Try to activate if activation policy is on spawn.
	if(ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// Torn off : ���̸� �� ���ʹ� �� �̻� �� Ŭ���̾�Ʈ�� �������� ������ �����Ǵ� Ŭ���̾�Ʈ���� "�и�"�ȴ�(ROLE_Authority�� ��).
		// �ƹ�Ÿ ���Ͱ� Torn off�ǰų� ������, �� �ƹ�Ÿ ���͸� ���� ������ Activate�� �õ����� �ʴ´�.
		if(ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

bool UDediServerTestGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	UDediTestAbilitySystemComponent* DediTestASC = CastChecked<UDediTestAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	const FTestGameplayTags& GameplayTags = FTestGameplayTags::Get();

	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	//@TODO Possibly remove after setting up tag relationships
	if(DediTestASC->IsActivationGroupBlocked(ActivationGroup))
	{
		/*if(OptionalRelevantTags)
		{
			//�����Ƽ ���н� ��Ȳ�� �±�
			OptionalRelevantTags->AddTag(GameplayTags.Ab)
		}*/
		return false;
	}

	return true;
}

void UDediServerTestGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UDediServerTestGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDediServerTestGameplayAbility::OnPawnAvatarSet()
{
	K2_OnPawnAvataSet();
}
