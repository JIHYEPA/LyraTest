// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "DediServerTestPawnComponent.generated.h"

UINTERFACE(BlueprintType)
class DEDISERVERTEST_API UDediServerTestReadyInterface : public UInterface
{
	GENERATED_BODY()
};

class IDediServerTestReadyInterface
{
	GENERATED_BODY()

public:
	virtual bool IsPawnComponentReadyToInitialize() const = 0;
};

/**
 * 
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class DEDISERVERTEST_API UDediServerTestPawnComponent : public UPawnComponent, public IDediServerTestReadyInterface
{
	GENERATED_BODY()

public:

	UDediServerTestPawnComponent(const FObjectInitializer& ObjectInitializer);

	virtual bool IsPawnComponentReadyToInitialize() const override { return true; }
};
