// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/DediServerTestAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Actor/DediServerTestCharacter.h"
#include "Net/UnrealNetwork.h"

UDediServerTestAttributeSet::UDediServerTestAttributeSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
{
}

void UDediServerTestAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDediServerTestAttributeSet, Health, OldValue);
}

void UDediServerTestAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UDediServerTestAttributeSet, MaxHealth, OldValue);
}

void UDediServerTestAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDediServerTestAttributeSet, Health);
	DOREPLIFETIME(UDediServerTestAttributeSet, MaxHealth);
}

void UDediServerTestAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetMaxHealthAttribute())
	{
		AdjestAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
}

void UDediServerTestAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	//UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	//const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	float DeltaValue { 0.f };

	// Clamp되기 전 Attribute 최종값
	if(Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	ADediServerTestCharacter* TargetCharacter { nullptr };

	// Attribute 값 주인 찾기
	if(Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetActor { nullptr };
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetCharacter = Cast<ADediServerTestCharacter>(TargetActor);
	}

	// Clamp해서 적용. Health값이 변경 됐을 때의 액션들 불러오기
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		if(TargetCharacter)
		{
			//TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
}

void UDediServerTestAttributeSet::AdjestAttributeForMaxChange(const FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
	const FGameplayAttribute& AffectedAttributeProperty) const
{
	UAbilitySystemComponent* AbilityComponent = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComponent)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		const float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComponent->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

