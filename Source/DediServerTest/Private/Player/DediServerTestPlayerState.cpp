// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DediServerTestPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Player/DediServerTestPlayerController.h"
#include "AbilitySystem/DediTestAbilitySystemComponent.h"
#include "Player/DediServerTestPawnData.h"
#include "AbilitySystem/DediServerTestAbilitySet.h"

ADediServerTestPlayerState::ADediServerTestPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UDediTestAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	bAbilityInitialized = false;
}

void ADediServerTestPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);

}

ADediServerTestPlayerController* ADediServerTestPlayerState::GetDediServerTestPlayerController() const
{
	return Cast<ADediServerTestPlayerController>(GetOwner());
}

void ADediServerTestPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ADediServerTestPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	///*GiveAbility*/
	//if (GetLocalRole() == ROLE_Authority && !bAbilityInitialized)
	//{
	//	if(PawnData && PawnData->IsValidLowLevelFast())
	//	{
	//		for (const UDediServerTestAbilitySet* AbilitySet : PawnData->AbilitySets)
	//		{
	//			if (AbilitySet)
	//			{
	//				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	//			}

	//			bAbilityInitialized = true;
	//		}
	//	}
	//}
}

void ADediServerTestPlayerState::OnRep_PawnData()
{
}
