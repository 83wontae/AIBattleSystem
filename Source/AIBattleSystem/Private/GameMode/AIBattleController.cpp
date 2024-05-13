// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AIBattleController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameMode/AIBattleSystemCharacter.h"

AAIBattleController::AAIBattleController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 500.0f;
	SightConfig->LoseSightRadius = 1000.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	//SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	//SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	AIPerception->SetDominantSense(*SightConfig->GetSenseImplementation());
	AIPerception->ConfigureSense(*SightConfig);
}

void AAIBattleController::BeginPlay()
{
	Super::BeginPlay();

	AAIBattleSystemCharacter* pChar = Cast<AAIBattleSystemCharacter>(GetPawn());
	if (IsValid(pChar))
	{
		TeamId = FGenericTeamId(pChar->m_ID);
	}

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

ETeamAttitude::Type AAIBattleController::GetTeamAttitudeTowards(const AActor& Other) const
{
	// Check if Pawn
	const APawn* OtherPawn = Cast<APawn>(&Other);
	if (IsValid(OtherPawn) == false)
	{return ETeamAttitude::Neutral;}

	// Check if Actor implements GenericTeamAgentInterface
	auto PlayerTI = Cast<IGenericTeamAgentInterface>(&Other);
	class IGenericTeamAgentInterface* BotTI = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
	if (BotTI == nullptr && PlayerTI == nullptr)//APawn에 있을수도 있고, APawn을 상속한 다른 Class에 있을수 있기 때문에 둘다 채크
	{return ETeamAttitude::Neutral;}

	// Get Actor's TeamId
	FGenericTeamId OtherActorTeamId = NULL;//0
	if (BotTI != nullptr)
	{OtherActorTeamId = BotTI->GetGenericTeamId();}
	else if (PlayerTI != nullptr)
	{OtherActorTeamId = PlayerTI->GetGenericTeamId();}

	// Check If Hostile
	FGenericTeamId ThisId = GetGenericTeamId();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("ThisId=%d, OtherId=%d"), ThisId, OtherActorTeamId));
	if (OtherActorTeamId == ThisId)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Friendly"));
		return ETeamAttitude::Friendly;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hostile"));
		return ETeamAttitude::Hostile;
	}
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
