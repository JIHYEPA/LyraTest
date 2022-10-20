// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"
#include "TestGameplayTag.h"
#include "DediServerTestCharacter.generated.h"

class UDediTestAbilitySystemComponent;
class ADediServerTestPlayerState;
class UDediTestPawnExtensionComponent;
struct FInputActionValue;

UCLASS(config=Game)
class ADediServerTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ADediServerTestCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "Lyra|Character")
	ADediServerTestPlayerState* GetDediServerTestPlayerState() const;

	UFUNCTION(BlueprintPure, Category = "DediServerTest|Character")
	UDediTestAbilitySystemComponent* GetDediTestAbilitySystemComponent() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& GameplayTag);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", meta = (AllowPrivateAccess = "true"))
	UDediTestPawnExtensionComponent* PawnExtComponent;

protected:

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	
};

