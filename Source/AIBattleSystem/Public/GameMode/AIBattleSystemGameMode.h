// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AIBattleSystemGameMode.generated.h"

UCLASS(minimalapi)
class AAIBattleSystemGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAIBattleSystemGameMode();

public:
	uint32 GetSeed() { m_Seed += 1; return m_Seed; };

private:
	uint32 m_Seed;
};



