// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

DEDISERVERTEST_API DECLARE_LOG_CATEGORY_EXTERN(LogDediServerTestAbilitySystem, Log, All);
DEDISERVERTEST_API DECLARE_LOG_CATEGORY_EXTERN(LogDediServerTestExperience, Log, All);
DEDISERVERTEST_API DECLARE_LOG_CATEGORY_EXTERN(LogDediServerTest, Log, All);

DEDISERVERTEST_API FString GetClientServerContextString(UObject* ContextObject = nullptr);