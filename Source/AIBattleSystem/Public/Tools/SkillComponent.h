// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/AIBattleController.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"



UENUM(BlueprintType)
enum class EN_SkillType : uint8
{
	Attack UMETA(Display = "Attack"),
	Defence UMETA(Display = "Defence"),
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
		: Type(EN_SkillType::Attack)
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	TArray<FName> DodgeSkill;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIBATTLESYSTEM_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	ACharacter* m_pOwnChar;

	//---[AI Skill & Move Area]---//
public:
	void TickAI(class AAIBattleController* pCtrl, float DeltaSeconds);

	bool IsUseingDefenceSkill(ACharacter* pChar);

	FST_AISkill* GetUseingSkill();

	FST_AISkill* GetUseingSkill(ACharacter* pChar);

	UFUNCTION()
	void OnEventMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEventBeginAttack();

	void OnEventBeginAttack_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEventBeforeHitNotify(ACharacter* pAttackChar);

	void OnEventBeforeHitNotify_Implementation(ACharacter* pAttackChar);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEventHitNotify(ACharacter* pAttackChar);

	void OnEventHitNotify_Implementation(ACharacter* pAttackChar);

public:
	UFUNCTION(BlueprintCallable)
	void SetSkill_AT(TArray<FName> names);

	UFUNCTION(BlueprintCallable)
	void UseSkill();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActivatedDefenceSkill();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActivatedAttackSkill();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ACharacter* GetTargetCharacter();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* AT_Table;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* DF_Table;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<FName> Skill_AT_Names;

	TArray<FST_AISkill*> m_Skill_ATs;

	TArray<FST_AISkill*> m_Skill_DFs;

	FRandomStream m_Stream;

	UPROPERTY(BlueprintReadOnly)
	EN_AIState m_AI_State;

	// 실행하고 있는 스킬
	FST_AISkill* stUsingSkill;
};
