// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DediServerTestPlayerState.generated.h"

class ADediServerTestPlayerController;
class UDediTestAbilitySystemComponent;
class UDediServerTestPawnData;

/**
 * 
 */
UCLASS()
class DEDISERVERTEST_API ADediServerTestPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	ADediServerTestPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "DediServerTest|PlayerState")
	ADediServerTestPlayerController* GetDediServerTestPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "DediServerTest|PlayerState")
	UDediTestAbilitySystemComponent* GetDediTestAbilitySystemComponent() const { return AbilitySystemComponent; }

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	template <class T>
	const T* GetPawnData() const  { return Cast<T>(PawnData); }

protected:
	UFUNCTION()
	void OnRep_PawnData();

private:

	UPROPERTY(VisibleAnywhere, Category = "DediServerTest|PlayerState")
	UDediTestAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, ReplicatedUsing= OnRep_PawnData, Category = "DediServerTest|Data")
	const UDediServerTestPawnData* PawnData;

	bool bAbilityInitialized;
};
