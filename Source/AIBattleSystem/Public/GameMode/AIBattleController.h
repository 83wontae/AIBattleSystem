// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBattleController.generated.h"

UENUM(BlueprintType)
enum class EN_AIState : uint8
{
	Patrol,
	Chase,
	Battle
};

/**
 * 
 */
UCLASS()
class AIBATTLESYSTEM_API AAIBattleController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIBattleController();

	virtual void BeginPlay();

	bool InitAI();

public:
	UFUNCTION(BlueprintCallable)
	void EventPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

public:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere)
	UAIPerceptionComponent* AIPerception;

	class UAISenseConfig_Sight* SightConfig;
};
