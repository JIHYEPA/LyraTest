// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DediServerTestPawnData.generated.h"

class UDediServerTestAbilitySet;
class UTestInputConfig;

/**
 * 
 */
UCLASS(BlueprintType, Const)
class DEDISERVERTEST_API UDediServerTestPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UDediServerTestPawnData(const FObjectInitializer& ObjectInitializer);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DediServerTest|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DediServerTest|Abilities")
	TArray<UDediServerTestAbilitySet*> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DediServerTest|Input")
	UTestInputConfig* InputConfig;

};
