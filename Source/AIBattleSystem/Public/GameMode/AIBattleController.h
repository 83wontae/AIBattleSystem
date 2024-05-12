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

UENUM(BlueprintType)
enum class EN_SkillType : uint8
{
	Active UMETA(Display = "Active"),
	Passive UMETA(Display = "Passive"),
	Buff UMETA(Display = "Buff"),
};

// struct 이름 앞에 반드시 F가 붙어야함
USTRUCT(BlueprintType)
struct FST_AISkill : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FST_AISkill()
		: Type(EN_SkillType::Active)
		, Name("")
		, Anim(nullptr)
		, StaminaUse(0.0f)
		, Damage(0.0f)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	EN_SkillType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* Anim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	float StaminaUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	float Damage;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere)
	UAIPerceptionComponent* AIPerception;

	class UAISenseConfig_Sight* SightConfig;
};
