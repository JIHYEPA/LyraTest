// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DediServerTestGameplayAbility.generated.h"

class ADediServerTestCharacter;

/**
 * �����Ƽ Ȱ��ȭ ��å
 */
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	//�Է� Ʈ���� ��. ���� �Է� �±װ� Ʈ���ŵǴ� ��� �����Ƽ �� �� Ȱ��ȭ. ��ư�� ������ �����Ƽ�� �� �� Ȱ��ȭ�ǰ� �ڵ����� ��Ȱ��ȭ���� ����.
	OnInputTriggered,

	//�Է� Ȱ��ȭ ��. ���� �Է� �±װ� Ʈ���ŵǴ� �� ����ؼ� �����Ƽ Ȱ��ȭ. 
	WhileInputActive,

	//���� ��. ���� ���� ������ ������ �����Ƽ�� ������� ����.
	OnSpawn
};

/**
 * �����Ƽ Ȱ��ȭ �׷� : �ٸ� �����Ƽ�� ���� ����
 */
UENUM(BlueprintType)
enum class EAbilityActivationGroup : uint8
{
	//����. �ٸ� �����Ƽ�� �����ϰų� ��ü���� ����. Default
	Independent,

	//���� ��ü ����. �ٸ� �����Ƽ�� �������� ������, �� �ٸ� ���� �����Ƽ�� Ȱ��ȭ�Ǹ� ��ҵ�.
	Exclusive_Replaceable,

	//���� ����. �� �����Ƽ�� ����Ǵ� ����, �ٸ� �����Ƽ�� Ȱ��ȭ�� �� ����.
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
