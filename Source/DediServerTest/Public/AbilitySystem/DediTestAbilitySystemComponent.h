// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "AbilitySystem/Abilities/DediServerTestGameplayAbility.h"
#include "DediTestAbilitySystemComponent.generated.h"

class UDediServerTestGameplayAbility;

DEDISERVERTEST_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

/**
 * 
 */
UCLASS()
class DEDISERVERTEST_API UDediTestAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UDediTestAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	typedef TFunctionRef<bool(const UDediServerTestGameplayAbility* DediServerTestAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
	void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicationCancelAbility);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();

	bool IsActivationGroupBlocked(EAbilityActivationGroup Group) const;
	void AddAbilityToActivationGroup(EAbilityActivationGroup Group, UDediServerTestGameplayAbility* DediServerTestAbility);
	void RemoveAbilityFromActivationGroup(EAbilityActivationGroup Group, UDediServerTestGameplayAbility* DediServerTestAbility);
	void CancelActivationGroupAbilities(EAbilityActivationGroup Group, UDediServerTestGameplayAbility* IgnoreDediServerTestAbility, bool bReplicateCancelAbility);

protected:

	void TryActivateAbilitiesOnSpawn();

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

protected:

	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	int32 ActivationGroupCounts[(uint8)EAbilityActivationGroup::MAX];
};
