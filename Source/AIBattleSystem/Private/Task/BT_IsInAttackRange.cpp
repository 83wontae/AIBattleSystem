// Fill out your copyright notice in the Description page of Project Settings.


#include "Task/BT_IsInAttackRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBT_IsInAttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Controller = Cast<AAIController>(OwnerComp.GetAIOwner());

	auto Enemy = Controller->GetPawn();
	if (nullptr == Controller)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Enemy Initialize faild!"));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr == BlackboardComp)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BlackboardComp Initialize faild!"));
		return EBTNodeResult::Failed;
	}

	AActor* pTargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActor.SelectedKeyName));
	if (nullptr == pTargetActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("TargetActor is not set!"));
		return EBTNodeResult::Failed;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, 
		FString::Printf(TEXT("TargetActor is %s"), *AActor::GetDebugName(pTargetActor)));

	auto length = FVector::Distance(pTargetActor->GetActorLocation(), Enemy->GetActorLocation());
	if(100 < length)
		return EBTNodeResult::Failed;

	return EBTNodeResult::Succeeded;
}
