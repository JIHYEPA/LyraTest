// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DediServerTestAbilitySet.h"
#include "AbilitySystem/DediTestAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/DediServerTestGameplayAbility.h"
#include "DediServerTestLogChannels.h"

void FDediServerTestAbilitySet_GrantedHandle::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if(Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FDediServerTestAbilitySet_GrantedHandle::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if(Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FDediServerTestAbilitySet_GrantedHandle::AddAttributeSet(UAttributeSet* Set)
{
	if(Set != nullptr)
	{
		GrantedAttributeSets.Add(Set);
	}
}

void FDediServerTestAbilitySet_GrantedHandle::TakeFromAbilitySystem(UDediTestAbilitySystemComponent* DediServerTestASC)
{
	check(DediServerTestASC);

	if(!DediServerTestASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for(const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if(Handle.IsValid())
		{
			DediServerTestASC->ClearAbility(Handle);
		}
	}

	for(const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if(Handle.IsValid())
		{
			DediServerTestASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for(UAttributeSet* Set : GrantedAttributeSets)
	{
		DediServerTestASC->GetSpawnedAttributes_Mutable().Remove(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UDediServerTestAbilitySet::UDediServerTestAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UDediServerTestAbilitySet::GiveToAbilitySystem(UDediTestAbilitySystemComponent* DediServerTestASC,
	FDediServerTestAbilitySet_GrantedHandle* OutGrantedHandles, UObject* SourceObject) const
{
	check(DediServerTestASC);

	if (!DediServerTestASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	//Gameaplay Ability 등록
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FDediServerTestAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UDediServerTestGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UDediServerTestGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = DediServerTestASC->GiveAbility(AbilitySpec);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	//Gameaplay Effect 등록
	for(int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FDediServerTestAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if(!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = DediServerTestASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, DediServerTestASC->MakeEffectContext());

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	//Attribute Set 등록
	for(int32 SetIndex = 0; SetIndex < Grantedattributes.Num(); ++SetIndex)
	{
		const FDediServerTestAbilitySet_AttributeSet& SetToGrant = Grantedattributes[SetIndex];

		if(!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogDediServerTestAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(DediServerTestASC->GetOwner(), SetToGrant.AttributeSet);
		DediServerTestASC->AddAttributeSetSubobject(NewSet);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}
