// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "Engine/DataAsset.h"
#include "DediServerTestAbilitySet.generated.h"

class UDediTestAbilitySystemComponent;
class UDediServerTestGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDediServerTestAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDediServerTestGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

USTRUCT(BlueprintType)
struct FDediServerTestAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 EffectLevel = 1;
};

USTRUCT(BlueprintType)
struct FDediServerTestAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;

};

USTRUCT(BlueprintType)
struct FDediServerTestAbilitySet_GrantedHandle
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UDediTestAbilitySystemComponent* DediServerTestASC);

protected:

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * 
 */
UCLASS(BlueprintType, Const)
class DEDISERVERTEST_API UDediServerTestAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UDediServerTestAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GiveToAbilitySystem(UDediTestAbilitySystemComponent* DediServerTestASC, FDediServerTestAbilitySet_GrantedHandle* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FDediServerTestAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = GameplayEffect))
	TArray<FDediServerTestAbilitySet_GameplayEffect> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = AttributeSet))
	TArray<FDediServerTestAbilitySet_AttributeSet> Grantedattributes;
};
