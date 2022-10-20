// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/DediTestPawnExtensionComponent.h"
#include "DediServerTestLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Player/DediServerTestPawnData.h"
#include "Actor/DediServerTestCharacter.h"
#include "AbilitySystem/DediTestAbilitySystemComponent.h"
#include "AbilitySystem/DediServerTestAbilitySet.h"

UDediTestPawnExtensionComponent::UDediTestPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
	bPawnReadyToInitialize = false;
}

void UDediTestPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDediTestPawnExtensionComponent, PawnData);
}

void UDediTestPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();
}

void UDediTestPawnExtensionComponent::SetPawnData(const UDediServerTestPawnData* InPawnData)
{
	check(InPawnData);

	bPawnReadyToInitialize = false;

	APawn* Pawn = GetPawnChecked<APawn>();

	if(Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if(PawnData)
	{
		UE_LOG(LogDediServerTest, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckPawnReadyToInitialize();
}

void UDediTestPawnExtensionComponent::OnRep_PawnData()
{
	CheckPawnReadyToInitialize();
}

void UDediTestPawnExtensionComponent::InitializeAbilitySystem(UDediTestAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if(AbilitySystemComponent == InASC)
	{
		return;
	}

	if(AbilitySystemComponent)
	{
		UnInitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogDediServerTest, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s]"), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogDediServerTest, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		ensure(!ExistingAvatar->HasAuthority());

		if(UDediTestPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UnInitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	ADediServerTestCharacter* OwnerCharacter = CastChecked<ADediServerTestCharacter>(Pawn);
	AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(OwnerCharacter, &ADediServerTestCharacter::OnAbilityFailed);

	if (Pawn->GetLocalRole() == ROLE_Authority)
	{
		if (PawnData && PawnData->IsValidLowLevelFast())
		{
			for (const UDediServerTestAbilitySet* AbilitySet : PawnData->AbilitySets)
			{
				if (AbilitySet)
				{
					AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
				}
			}
		}
	}

	OnAbilitySystemInitialized.Broadcast();
}

void UDediTestPawnExtensionComponent::UnInitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if(AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent->CancelAbilities(nullptr, nullptr);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if(AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UDediTestPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UnInitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	CheckPawnReadyToInitialize();
}

void UDediTestPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckPawnReadyToInitialize();
}

void UDediTestPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckPawnReadyToInitialize();
}

bool UDediTestPawnExtensionComponent::CheckPawnReadyToInitialize()
{
	if(bPawnReadyToInitialize)
	{
		return true;
	}

	if(!PawnData)
	{
		return false;
	}

	APawn* Pawn = GetPawnChecked<APawn>();

	const bool bHasAuthority = Pawn->HasAuthority();
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	if (bHasAuthority || bIsLocallyControlled)
	{
		// Check for being possessed by a controller.
		if (!GetController<AController>())
		{
			return false;
		}
	}

	// Allow pawn components to have requirements.
	TArray<UActorComponent*> InteractableComponents = Pawn->GetComponentsByInterface(UDediServerTestReadyInterface::StaticClass());
	for (UActorComponent* InteractableComponent : InteractableComponents)
	{
		const IDediServerTestReadyInterface* Ready = CastChecked<IDediServerTestReadyInterface>(InteractableComponent);
		if(!Ready->IsPawnComponentReadyToInitialize())
		{
			return false;
		}
	}

	bPawnReadyToInitialize = true;
	OnPawnReadyToInitialize.Broadcast();
	BP_OnPawnReadyToInitialize.Broadcast();

	return true;
}

void UDediTestPawnExtensionComponent::OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if(!OnPawnReadyToInitialize.IsBoundToObject(Delegate.GetUObject()))
	{
		OnPawnReadyToInitialize.Add(Delegate);
	}

	if(bPawnReadyToInitialize)
	{
		Delegate.Execute();
	}
}

void UDediTestPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UDediTestPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}
