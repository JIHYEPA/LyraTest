// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DediServerTestGameplayAbility.generated.h"

class ADediServerTestCharacter;

/**
 * 어빌리티 활성화 정책
 */
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	//입력 트리거 시. 관련 입력 태그가 트리거되는 즉시 어빌리티 한 번 활성화. 버튼이 눌리면 어빌리티가 한 번 활성화되고 자동으로 재활성화되지 않음.
	OnInputTriggered,

	//입력 활성화 중. 관련 입력 태그가 트리거되는 한 계속해서 어빌리티 활성화. 
	WhileInputActive,

	//스폰 시. 폰이 빙의 해제될 때까지 어빌리티가 종료되지 않음.
	OnSpawn
};

/**
 * 어빌리티 활성화 그룹 : 다른 어빌리티와 관계 설정
 */
UENUM(BlueprintType)
enum class EAbilityActivationGroup : uint8
{
	//독립. 다른 어빌리티를 차단하거나 대체하지 않음. Default
	Independent,

	//전용 대체 가능. 다른 어빌리티를 차단하지 않지만, 또 다른 전용 어빌리티가 활성화되면 취소됨.
	Exclusive_Replaceable,

	//전용 차단. 이 어빌리티가 실행되는 동안, 다른 어빌리티가 활성화될 수 없음.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};

/**
 * 
 */
UCLASS(Abstract)
class DEDISERVERTEST_API UDediServerTestGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UDediTestAbilitySystemComponent;

public:

	UDediServerTestGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Ability")
	ADediServerTestCharacter* GetDediServerTestCharacterFromActorInfo() const;

	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void OnPawnAvatarSet();

	UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvataSet")
	void K2_OnPawnAvataSet();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EAbilityActivationPolicy ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EAbilityActivationGroup ActivationGroup;
};
