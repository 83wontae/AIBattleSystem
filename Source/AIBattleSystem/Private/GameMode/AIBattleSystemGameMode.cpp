// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode/AIBattleSystemGameMode.h"
#include "GameMode/AIBattleSystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAIBattleSystemGameMode::AAIBattleSystemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
