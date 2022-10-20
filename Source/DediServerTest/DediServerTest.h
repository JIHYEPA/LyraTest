// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FDediServerTestGameModule final : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;

	virtual void ShutdownModule() override
	{
	}
};