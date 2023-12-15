// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AIBattleController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"

AAIBattleController::AAIBattleController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 500.0f;
	SightConfig->LoseSightRadius = 1000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->SetDominantSense(*SightConfig->GetSenseImplementation());
	AIPerception->ConfigureSense(*SightConfig);
}

void AAIBattleController::BeginPlay()
{
	Super::BeginPlay();

	InitAI();
}

bool AAIBattleController::InitAI()
{
	if (IsValid(BehaviorTree) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BehaviorTree is not set!"));
		return false;
	}

	RunBehaviorTree(BehaviorTree);

	EN_AIState aistate = (EN_AIState)GetBlackboardComponent()->GetValueAsEnum("AiState");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Aistate = %d"), aistate));

	// RunBehaviorTree 후 AIPerception 이벤트에 바인딩을 해줘야함
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AAIBattleController::EventPerceptionUpdated);
	return true;
}

void AAIBattleController::EventPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("EventPerceptionUpdated"));
	
	if (false == Stimulus.WasSuccessfullySensed())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Stimulus.WasSuccessfullySensed() = false"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Stimulus.WasSuccessfullySensed() = true"));

	ACharacter* pChar = Cast<ACharacter>(Actor);
	if (nullptr == pChar)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("pChar = nullptr"));
		return;
	}

	//EN_AIState aistate = (EN_AIState)GetBlackboardComponent()->GetValueAsEnum("AiState");
	GetBlackboardComponent()->SetValueAsEnum("AiState", (uint8)EN_AIState::Chase);

	GetBlackboardComponent()->SetValueAsObject("TargetActor", Actor);
}
