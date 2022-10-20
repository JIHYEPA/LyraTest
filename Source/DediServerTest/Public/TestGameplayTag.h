// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;
/**
 * 
 */
struct DEDISERVERTEST_API FTestGameplayTags : public FNoncopyable
{
public:

	static const FTestGameplayTags& Get() { return GameplayTag; }

	static void InitializeNativeTags();

	FORCEINLINE const FGameplayTag& Move() const { return InputTag_Move; }
	FORCEINLINE const FGameplayTag& LookMouse() const { return InputTag_Look_Mouse; }
	FORCEINLINE const FGameplayTag& Jump() const { return InputTag_Jump; }

protected:

	void AddAllTags(UGameplayTagsManager& Manager);

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:

	//Input Tag
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Jump;

	static FTestGameplayTags GameplayTag;
};
