// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/DediServerTestPawnComponent.h"

UDediServerTestPawnComponent::UDediServerTestPawnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}
