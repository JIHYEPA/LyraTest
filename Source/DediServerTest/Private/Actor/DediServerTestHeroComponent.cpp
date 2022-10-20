// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/DediServerTestHeroComponent.h"
#include "DediServerTestLogChannels.h"
#include "TestGameplayTag.h"
#include "Player/DediServerTestPlayerState.h"
#include "Player/DediServerTestPawnData.h"
#include "Actor/DediTestPawnExtensionComponent.h"
#include "Player/DediServerTestPlayerController.h"
#include "Input/DediServerTestInputComponent.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/DediTestAbilitySystemComponent.h"
#include "Actor/DediServerTestCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Settings/DediTestSettingLocal.h"
//#include "Components/GameFrameworkComponentManager.h"
//#include "EnhancedInputSubsystems.h"

UDediServerTestHeroComponent::UDediServerTestHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bPawnHasInitialized = false;
	bReadyToBindInputs = false;
}

bool UDediServerTestHeroComponent::HasPawnInitialized() const
{
	return bPawnHasInitialized;
}

bool UDediServerTestHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UDediServerTestHeroComponent::OnRegister()
{
	Super::OnRegister();

	if(const APawn* Pawn = GetPawn<APawn>())
	{
		if(UDediTestPawnExtensionComponent* PawnExtComp = UDediTestPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnExtComp->OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnPawnReadyToInitialize));
		}
	}
	else
	{
		UE_LOG(LogDediServerTest, Error, TEXT("[UDediServerTestHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));
	}
}

bool UDediServerTestHeroComponent::IsPawnComponentReadyToInitialize() const
{
	if(!GetPlayerState<ADediServerTestPlayerState>())
	{
		return false;
	}

	const APawn* Pawn = GetPawn<APawn>();

	if(!Pawn)
	{
		return false;
	}

	if(Pawn->GetLocalRole() != ROLE_SimulatedProxy)
	{
		AController* Controller = GetController<AController>();

		const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
												(Controller->PlayerState != nullptr) && \
												(Controller->PlayerState->GetOwner() == Controller);

		if (!bHasControllerPairedWithPS)
		{
			return false;
		}
	}

	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
	const bool bIsBot = Pawn->IsBotControlled();

	if (bIsLocallyControlled && !bIsBot)
	{
		if (!Pawn->InputComponent)
		{
			return false;
		}
	}

	return true;
}

void UDediServerTestHeroComponent::OnPawnReadyToInitialize()
{
	if(!ensure(!bPawnHasInitialized))
	{
		// 두번 초기화하지 않음.
		return;
	}

	APawn* Pawn = GetPawn<APawn>();
	if(!Pawn)
	{
		return;
	}

	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	ADediServerTestPlayerState* DediServerTestPS = GetPlayerState<ADediServerTestPlayerState>();
	check(DediServerTestPS);

	const UDediServerTestPawnData* PawnData = nullptr;

	if(UDediTestPawnExtensionComponent* PawnExtComp = UDediTestPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		PawnData = PawnExtComp->GetPawnData<UDediServerTestPawnData>();

		PawnExtComp->InitializeAbilitySystem(DediServerTestPS->GetDediTestAbilitySystemComponent(), DediServerTestPS);
	}

	if(ADediServerTestPlayerController* DediServerTestPC = GetController<ADediServerTestPlayerController>())
	{
		if(Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}

	bPawnHasInitialized = true;
}

void UDediServerTestHeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDediServerTestHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (UDediTestPawnExtensionComponent* PawnExtComp = UDediTestPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnExtComp->UnInitializeAbilitySystem();
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UDediServerTestHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if(!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if(const UDediTestPawnExtensionComponent* PawnExtComp = UDediTestPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if(const UDediServerTestPawnData* PawnData = PawnExtComp->GetPawnData<UDediServerTestPawnData>())
		{
			if(const UTestInputConfig* InputConfig = PawnData->InputConfig)
			{
				const FTestGameplayTags& GameplayTags = FTestGameplayTags::Get();

				UDediServerTestInputComponent* DediServerTestIC = CastChecked<UDediServerTestInputComponent>(PlayerInputComponent);
				DediServerTestIC->AddInputMappings(InputConfig, Subsystem);
				if(UDediTestSettingLocal* LocalSettings = UDediTestSettingLocal::Get())
				{
					LocalSettings->OnInputConfigActivated.AddUObject(this, &ThisClass::OnInputConfigActivated);
					LocalSettings->OnInputConfigDeactivated.AddUObject(this, &ThisClass::OnInputConfigDeactivated);
				}

				TArray<uint32> BindHandles;
				DediServerTestIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

				DediServerTestIC->BindActionByTag(InputConfig, GameplayTags.Move(), ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
				DediServerTestIC->BindActionByTag(InputConfig, GameplayTags.LookMouse(), ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
				//DediServerTestIC->BindActionByTag(InputConfig, GameplayTags.Jump(), ETriggerEvent::Triggered, this, &ThisClass::Input_Jump);
			}
		}
	}

	if(ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	//UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow); 게임피쳐관련
}

void UDediServerTestHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UDediTestPawnExtensionComponent* PawnExtComp = UDediTestPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if(UDediTestAbilitySystemComponent* DediTestASC = PawnExtComp->GetDediTestAbilitySystemCompnent())
			{
				DediTestASC->AbilityInputTagPressed(InputTag);
			}
		}
	}
}

void UDediServerTestHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UDediTestPawnExtensionComponent* PawnExtComp = UDediTestPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UDediTestAbilitySystemComponent* DediTestASC = PawnExtComp->GetDediTestAbilitySystemCompnent())
		{
			DediTestASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UDediServerTestHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller != nullptr)
	{
		const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (MoveValue.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, MoveValue.X);
		}

		if (MoveValue.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, MoveValue.Y);
		}
	}
}

void UDediServerTestHeroComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D LookValue = InputActionValue.Get<FVector2D>();

	if (LookValue.X != 0.0f)
	{
		Pawn->AddControllerYawInput(LookValue.X);
	}

	if (LookValue.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(LookValue.Y);
	}
}

void UDediServerTestHeroComponent::OnInputConfigActivated(const FLoadedMappableConfigPair& ConfigPair)
{
	if (ADediServerTestPlayerController* DediServerTestPC = GetController<ADediServerTestPlayerController>())
	{
		if (APawn* Pawn = GetPawn<APawn>())
		{
			if(UDediServerTestInputComponent* DediServerTestIC = CastChecked<UDediServerTestInputComponent>(Pawn->InputComponent))
			{
				if (const ULocalPlayer* LP = DediServerTestPC->GetLocalPlayer())
				{
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
					{
						DediServerTestIC->AddInputConfig(ConfigPair, Subsystem);
					}
				}
			}
		}
	}
}

void UDediServerTestHeroComponent::OnInputConfigDeactivated(const FLoadedMappableConfigPair& ConfigPair)
{
	if (ADediServerTestPlayerController* DediServerTestPC = GetController<ADediServerTestPlayerController>())
	{
		if (APawn* Pawn = GetPawn<APawn>())
		{
			if (UDediServerTestInputComponent* DediServerTestIC = CastChecked<UDediServerTestInputComponent>(Pawn->InputComponent))
			{
				if (const ULocalPlayer* LP = DediServerTestPC->GetLocalPlayer())
				{
					if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
					{
						DediServerTestIC->RemoveInputConfig(ConfigPair, Subsystem);
					}
				}
			}
		}
	}
}
