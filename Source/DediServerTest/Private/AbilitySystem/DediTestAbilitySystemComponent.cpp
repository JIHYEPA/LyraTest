// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DediTestAbilitySystemComponent.h"
#include "AbilitySystem/DediTestGlobalAbilitysystem.h"
#include "AbilitySystem/Abilities/DediServerTestGameplayAbility.h"
#include "DediServerTestLogChannels.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked");

UDediTestAbilitySystemComponent::UDediTestAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

void UDediTestAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UDediTestGlobalAbilitysystem* GlobalAbilitysystem = UWorld::GetSubsystem<UDediTestGlobalAbilitysystem>(GetWorld()))
	{
		GlobalAbilitysystem->UnRegisterASC(this);
	}
}

void UDediTestAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();

	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if(bHasNewPawnAvatar)
	{
		// 새로운 폰 아바타가 설정되었음을 모든 어빌리티에 알린다.
		for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			UDediServerTestGameplayAbility* DediServerTestAbilityCDO = CastChecked<UDediServerTestGameplayAbility>(AbilitySpec.Ability);

			if(DediServerTestAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
			{
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				for(UGameplayAbility* AbilityInstance : Instances)
				{
					UDediServerTestGameplayAbility* DediServerTestAbilityInstance = CastChecked<UDediServerTestGameplayAbility>(AbilityInstance);
					DediServerTestAbilityInstance->OnPawnAvatarSet();
				}
			}
			else
			{
				DediServerTestAbilityCDO->OnPawnAvatarSet();
			}
		}

		// 폰 아바타가 실제로 생기면 GlobalSystem에 등록한다. Global로 적용되는 일부 효과에는 아바타가 필요할 수 있으므로 이 때까지 기다린다.
		if(UDediTestGlobalAbilitysystem* GlobalAbilitysystem = UWorld::GetSubsystem<UDediTestGlobalAbilitysystem>(GetWorld()))
		{
			GlobalAbilitysystem->RegisterASC(this);
		}

		// DediServerTestAnimInstance Init
	}

	TryActivateAbilitiesOnSpawn();
}

void UDediTestAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicationCancelAbility)
{
	ABILITYLIST_SCOPE_LOCK();
	for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if(!AbilitySpec.IsActive())
		{
			continue;
		}

		UDediServerTestGameplayAbility* DediServerTestAbilityCDO = CastChecked<UDediServerTestGameplayAbility>(AbilitySpec.Ability);

		if(DediServerTestAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
		{
			// CDO가 아닌 생성된 모든 인스턴스를 취소한다.
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			for(UGameplayAbility* AbilityInstance : Instances)
			{
				UDediServerTestGameplayAbility* DediServerTestAbilityInstance = CastChecked<UDediServerTestGameplayAbility>(AbilityInstance);

				if(ShouldCancelFunc(DediServerTestAbilityInstance, AbilitySpec.Handle))
				{
					if(DediServerTestAbilityInstance->CanBeCanceled())
					{
						DediServerTestAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), DediServerTestAbilityInstance->GetCurrentActivationInfo(), bReplicationCancelAbility);
					}
					else
					{
						UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("CancelAbilitiesByFunc : Can't cancel ability [%s] because CanBeCanceled is false."), *DediServerTestAbilityInstance->GetName());
					}
				}
			}
		}
		else
		{
			// 인스턴스가 아닌 Ability CDO를 취소한다.
			if(ShouldCancelFunc(DediServerTestAbilityCDO, AbilitySpec.Handle))
			{
				//인스턴스가 아닌 Ability들은 항상 취소된다.
				check(DediServerTestAbilityCDO->CanBeCanceled());
				DediServerTestAbilityCDO->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicationCancelAbility);
			}
		}
	}
}

void UDediTestAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if(InputTag.IsValid())
	{
		for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact((InputTag))))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UDediTestAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UDediTestAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	ABILITYLIST_SCOPE_LOCK();

	if(HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// 입력이 유지될 때 활성화되는 모든 Ability를 처리
	for(const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if(const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UDediServerTestGameplayAbility* DediServerTestAbilityCDO = CastChecked<UDediServerTestGameplayAbility>(AbilitySpec->Ability);

				if(DediServerTestAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// 이 프레임에서 입력을 받은 모든 Ability를 처리
	for(const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if(FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if(AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				//

				TArray<FGameplayTag> AllDynamicTags;

				AbilitySpec->DynamicAbilityTags.GetGameplayTagArray(AllDynamicTags);

				for (FGameplayTag DynamicTag : AllDynamicTags)
				{
					UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("ProcessAbilityInput: Pressed [%s] ability, TagName : [%s]"), *AbilitySpec->Ability->GetName(), *DynamicTag.GetTagName().ToString());
				}

				//

				if(AbilitySpec->IsActive())
				{
					//Ability가 활성화되어 있으므로 입력 이벤트로 전달
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UDediServerTestGameplayAbility* DediServerTestAbilityCDO = CastChecked<UDediServerTestGameplayAbility>(AbilitySpec->Ability);

					if (DediServerTestAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// Press 및 Hold의 모든 Ability를 활성화
	// 보류된 입력이 Ability를 활성화하지 않도록 모든 작업을 한 번에 수행
	// Press로 인해 Ability에 입력 이벤트를 보냄.
	for(const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		bool bTryActiveAbility = TryActivateAbility(AbilitySpecHandle);

		if(AbilityActorInfo->AvatarActor != nullptr)
		{
			bool bHasAuthority = AbilityActorInfo->AvatarActor->HasAuthority();

			if (bTryActiveAbility)
			{
				UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("[%s] TryActivateAbility::true"), *AbilitySpecHandle.ToString());
			}
			else
			{
				if (bHasAuthority)
				{
					UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("[%s] TryActivateAbility::false :: Server"), *AbilitySpecHandle.ToString());
				}
				else
				{
					UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("[%s] TryActivateAbility::false :: Client"), *AbilitySpecHandle.ToString());
				}
			}
		}
	}

	// 이 프레임에서 입력이 해제된 모든 Ability를 처리
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					//Ability가 활성화되어 있으므로 입력 이벤트로 전달
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UDediTestAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

bool UDediTestAbilitySystemComponent::IsActivationGroupBlocked(EAbilityActivationGroup Group) const
{
	bool bBlocked = false;

	switch (Group)
	{
	case EAbilityActivationGroup::Independent:
		bBlocked = false;
		break;
	case EAbilityActivationGroup::Exclusive_Replaceable:
	case EAbilityActivationGroup::Exclusive_Blocking:
		bBlocked = (ActivationGroupCounts[(uint8)EAbilityActivationGroup::Exclusive_Blocking] > 0);
		break;
	default:
		checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	return bBlocked;
}

void UDediTestAbilitySystemComponent::AddAbilityToActivationGroup(EAbilityActivationGroup Group,
	UDediServerTestGameplayAbility* DediServerTestAbility)
{
	check(DediServerTestAbility);
	check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

	ActivationGroupCounts[(uint8)Group]++;

	const bool bReplicateCancelAbility = false;

	switch (Group)
	{
	case EAbilityActivationGroup::Independent:
		break;

	case EAbilityActivationGroup::Exclusive_Replaceable:
	case EAbilityActivationGroup::Exclusive_Blocking:
		CancelActivationGroupAbilities(EAbilityActivationGroup::Exclusive_Replaceable, DediServerTestAbility, bReplicateCancelAbility);
		break;

	default:
		checkf(false, TEXT("AddAbilityToActivationGroup : Invalid ActivationGroup [%d]\n"), (uint8)Group);
		break;
	}

	const int32 ExclusiveCount = ActivationGroupCounts[(uint8)EAbilityActivationGroup::Exclusive_Replaceable] + ActivationGroupCounts[(uint8)EAbilityActivationGroup::Exclusive_Blocking];
	if(!ensure(ExclusiveCount <= 1))
	{
		UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("AddAbilityToActivationGroup : Multiple exclusive abilities are running."));
	}
}

void UDediTestAbilitySystemComponent::RemoveAbilityFromActivationGroup(EAbilityActivationGroup Group,
	UDediServerTestGameplayAbility* DediServerTestAbility)
{
	check(DediServerTestAbility);
	check(ActivationGroupCounts[(uint8)Group] > 0);

	ActivationGroupCounts[(uint8)Group]--;
}

void UDediTestAbilitySystemComponent::CancelActivationGroupAbilities(EAbilityActivationGroup Group,
	UDediServerTestGameplayAbility* IgnoreDediServerTestAbility, bool bReplicateCancelAbility)
{
	TShouldCancelAbilityFunc ShouldCancelFunc = [this, Group, IgnoreDediServerTestAbility](const UDediServerTestGameplayAbility* DediServerTestAbility, FGameplayAbilitySpecHandle Handle)
	{
		return ((DediServerTestAbility->GetActivationGroup() == Group) && (DediServerTestAbility != IgnoreDediServerTestAbility));
	};

	CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void UDediTestAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	ABILITYLIST_SCOPE_LOCK();

	for(const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const UDediServerTestGameplayAbility* DediServerTestAbilityCDO = CastChecked<UDediServerTestGameplayAbility>(AbilitySpec.Ability);
		DediServerTestAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}

void UDediTestAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if(Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UDediTestAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}
