// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayAbilitySpec.h"
#include "DediTestGlobalAbilitysystem.generated.h"

class UDediTestAbilitySystemComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;

USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<UDediTestAbilitySystemComponent*, FGameplayAbilitySpecHandle> Handles;

	void AddToASC(TSubclassOf<UGameplayAbility> Ability, UDediTestAbilitySystemComponent* ASC);
	void RemoveFromASC(UDediTestAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<UDediTestAbilitySystemComponent*, FActiveGameplayEffectHandle> Handles;

	void AddToASC(TSubclassOf<UGameplayEffect> Effect, UDediTestAbilitySystemComponent* ASC);
	void RemoveFromASC(UDediTestAbilitySystemComponent* ASC);
	void RemoveFromAll();
};

/**
 * 레벨의 모든 어빌리티 시스템 컴포넌트 추적 & 상호작용
 * ex1) 모든 플레이어 데미지 면역
 * ex2) 매치 시작 전이라는 것을 알리는 UI 활성화
 */
UCLASS()
class DEDISERVERTEST_API UDediTestGlobalAbilitysystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	UDediTestGlobalAbilitysystem();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "DediServerTest")
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "DediServerTest")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "DediServerTest")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "DediServerTest")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	void RegisterASC(UDediTestAbilitySystemComponent* ASC);

	void UnRegisterASC(UDediTestAbilitySystemComponent* ASC);

private:

	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TArray<UDediTestAbilitySystemComponent*> RegisteredASCs;

};
