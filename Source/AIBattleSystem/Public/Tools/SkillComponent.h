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

UENUM(BlueprintType)
enum class EN_BattleAnimState : uint8
{
	Stand UMETA(Display = "Stand"),
	Attack UMETA(Display = "Attack"),
	Dodge UMETA(Display = "Dodge"),
	BigDodge UMETA(Display = "BigDodge"),
	Block UMETA(Display = "Block"),
	BigBlock UMETA(Display = "BigBlock"),
	Hit UMETA(Display = "Hit"),
	BigHit UMETA(Display = "BigHit"),
	DefenseStance UMETA(Display = "DefenseStance"),
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
		, SkillAnimRow("")
		, StaminaUse(0.0f)
		, Damage(0.0f)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	EN_SkillType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	FName SkillAnimRow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	float StaminaUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	float Damage;
};

// struct 이름 앞에 반드시 F가 붙어야함
USTRUCT(BlueprintType)
struct FST_SkillAnim : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FST_SkillAnim()
		: AttackAnim(nullptr)
		, DodgeAnim(nullptr)
		, BigDodgeAnim(nullptr)
		, BlockAnim(nullptr)
		, BigBlockAnim(nullptr)
		, HitAnim(nullptr)
		, BigHitAnim(nullptr)
	{
	}

	UAnimMontage* GetSkillAnim(EN_BattleAnimState animState)
	{
		switch (animState)
		{
		case EN_BattleAnimState::Stand:				{ return nullptr; }				break;
		case EN_BattleAnimState::Attack:			{ return AttackAnim; }			break;
		case EN_BattleAnimState::Dodge:				{ return DodgeAnim; }			break;
		case EN_BattleAnimState::BigDodge:			{ return BigDodgeAnim; }		break;
		case EN_BattleAnimState::Block:				{ return BlockAnim; }			break;
		case EN_BattleAnimState::BigBlock:			{ return BigBlockAnim; }		break;
		case EN_BattleAnimState::Hit:				{ return HitAnim; }				break;
		case EN_BattleAnimState::BigHit:			{ return BigHitAnim; }			break;
		case EN_BattleAnimState::DefenseStance:		{ return DefenseStanceAnim; }	break;
		default: { return nullptr; }	break;
		}
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* DodgeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* BigDodgeAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* BlockAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* BigBlockAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* HitAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* BigHitAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillData")
	UAnimMontage* DefenseStanceAnim;
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

	//---[AI Skill & Move Area]---//
public:
	void TickAI(class AAIBattleController* pCtrl, float DeltaSeconds);

	bool IsUseingDefenceSkill(ACharacter* pChar);

	FST_AISkill* GetUseingSkill();

	FST_AISkill* GetUseingSkill(ACharacter* pChar);

	FST_SkillAnim* GetSkillAnim(FName Row);

	EN_BattleAnimState CalcDefenseState();

	float GetDefenseStamina(EN_BattleAnimState DefState);

	UFUNCTION()
	void OnEventMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 적에 공격 시작시 이벤트 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEventBeginAttack(ACharacter* pAttackChar);

	void OnEventBeginAttack_Implementation(ACharacter* pAttackChar);

	// 적에 공격 히트전 이벤트 
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEventBeforeHitNotify(ACharacter* pAttackChar);

	void OnEventBeforeHitNotify_Implementation(ACharacter* pAttackChar);

	// 적에 공격 히트 이벤트 
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
	ACharacter* m_pOwnChar;

	class UCharStateComponent* m_CharState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<FName> SkillList;

	TArray<FST_AISkill*> m_Skill_ATs;

	UPROPERTY(BlueprintReadOnly)
	EN_AIState m_CurAiState;

	// 실행하고 있는 스킬
	FST_AISkill* m_CurUsingSkill;

	// 현재 Anim 상태
	EN_BattleAnimState m_CurAnimState;

	// 낮을 수록 공격 주도권을 같는다.
	float AttackInitiative = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* SkillTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* SkillAnimTable;

	FRandomStream m_RandStream;
};
