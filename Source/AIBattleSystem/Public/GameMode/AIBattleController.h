// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBattleController.generated.h"

UENUM(BlueprintType)
enum class EN_AIState : uint8
{
	Patrol UMETA(Display = "Patrol"),
	Chase UMETA(Display = "Chase"),
	Battle UMETA(Display = "Battle"),
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

	//---[GenericTeamAgentInterface Area]---//
protected:
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

public:
	FGenericTeamId TeamId;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId;}

	//---[AI Move Area]---//
public:
	UFUNCTION(BlueprintCallable)
	void EventPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACharacter* GetTargetChar() { return m_pTargetChar; };

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere)
	UAIPerceptionComponent* AIPerception;

	class UAISenseConfig_Sight* SightConfig;

	ACharacter* m_pTargetChar;
};
