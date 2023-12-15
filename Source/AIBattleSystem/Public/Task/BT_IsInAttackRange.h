// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BT_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class AIBATTLESYSTEM_API UBT_IsInAttackRange : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActor;
};
