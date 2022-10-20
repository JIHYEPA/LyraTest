// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DediServerTestPawnData.h"
#include "AbilitySystem/DediServerTestAbilitySet.h"
#include "Input/TestInputConfig.h"

UDediServerTestPawnData::UDediServerTestPawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
}
