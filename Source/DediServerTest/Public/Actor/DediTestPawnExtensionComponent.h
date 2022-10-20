// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/DediServerTestPawnComponent.h"
#include "DediTestPawnExtensionComponent.generated.h"

class UDediServerTestPawnData;
class UDediTestAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDediTestDynamicMulticastDelegate);

/**
 * 
 */
UCLASS()
class DEDISERVERTEST_API UDediTestPawnExtensionComponent : public UDediServerTestPawnComponent
{
	GENERATED_BODY()

public:

	UDediTestPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	// 특정 Actor에 존재하는 Pawn Extension Component를 반환.
	UFUNCTION(BlueprintPure, Category="DediServerTest|Pawn")
	static UDediTestPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDediTestPawnExtensionComponent>() : nullptr); }

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UDediServerTestPawnData* InPawnData);

	UFUNCTION(BlueprintPure, Category = "DediServerTest|Pawn")
	UDediTestAbilitySystemComponent* GetDediTestAbilitySystemCompnent() const { return AbilitySystemComponent; }

	void InitializeAbilitySystem(UDediTestAbilitySystemComponent* InASC, AActor* InOwnerActor);

	void UnInitializeAbilitySystem();

	void HandleControllerChanged();

	void HandlePlayerStateReplicated();

	void SetupPlayerInputComponent();

	bool CheckPawnReadyToInitialize();

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "DediServerTest|Pawn", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsPawnReadyToInitialize() const { return bPawnReadyToInitialize; }

	void OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);

	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:

	virtual void OnRegister() override;

	UFUNCTION()
	void OnRep_PawnData();

	FSimpleMulticastDelegate OnPawnReadyToInitialize;

	UPROPERTY(BlueprintAssignable, Meta = (DisplayName = "On Pawn Ready To Initialize"))
	FDediTestDynamicMulticastDelegate BP_OnPawnReadyToInitialize;

	FSimpleMulticastDelegate OnAbilitySystemInitialized;

	FSimpleMulticastDelegate OnAbilitySystemUninitialized;

protected:

	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "DediServerTest|Pawn")
	const UDediServerTestPawnData* PawnData;

	UPROPERTY()
	UDediTestAbilitySystemComponent* AbilitySystemComponent;

	int32 bPawnReadyToInitialize : 1;
};
