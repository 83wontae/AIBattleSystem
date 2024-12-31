// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/SkillComponent.h"
#include "GameMode/AIBattleSystemCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameMode/AIBattleSystemGameMode.h"
#include "Tools/CharStateComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent():m_CurAnimState(EN_BattleAnimState::Stand)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bAutoActivate = true;
}


// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	m_pOwnChar = Cast<ACharacter>(GetOwner());
	if (IsValid(m_pOwnChar) == false)
		return;

	SetSkill_AT(SkillList);
	/*
	if (IsValid(DF_Table))
	{
		TArray<FST_AISkill*> arr;
		DF_Table->GetAllRows<FST_AISkill>(TEXT("GetAllRows"), arr);
		for (FST_AISkill* iter : arr)
		{
			m_Skill_DFs.Push(iter);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DF_Table is Empty"));
	}
	*/
	// Bind MontageEnded Event
	m_pOwnChar->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &USkillComponent::OnEventMontageEnded);

	// Set AI Random Seed 
	AAIBattleSystemGameMode* pGM = Cast<AAIBattleSystemGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(pGM))
	{
		int32 seed = pGM->GetSeed();
		m_RandStream.Initialize(seed);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("AI Seed = %d"), seed));
	}
	else
	{
		m_RandStream.Initialize(0);
	}
}


// Called every frame
void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (false == IsValid(m_pOwnChar))
		return;

	AAIBattleController* pCtrl = Cast<AAIBattleController>(m_pOwnChar->GetController());
	if (IsValid(pCtrl))
	{
		TickAI(pCtrl, DeltaTime);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Owner Character is not AAIBattleController"));
	}
}

void USkillComponent::TickAI(AAIBattleController* pCtrl, float DeltaSeconds)
{
	if (false == IsValid(pCtrl->GetBlackboardComponent()))
		return;

	m_CurAiState = (EN_AIState)pCtrl->GetBlackboardComponent()->GetValueAsEnum("AiState");
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Aistate = %d"), aistate));
	switch (m_CurAiState)
	{
	case EN_AIState::Patrol: {

	}	break;
	case EN_AIState::Chase: {

	}	break;
	case EN_AIState::Battle: {

		if (m_pOwnChar->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
			break;

		if (m_Skill_ATs.IsEmpty())
			break;
			
		UseSkill();
		
	}	break;
	default: {

	}
	}
}

bool USkillComponent::IsUseingDefenceSkill(ACharacter* pChar)
{
	// 상대방이 회피중인지 채크
	USkillComponent* targetSkillComp = pChar->FindComponentByClass<USkillComponent>();
	if (false == IsValid(targetSkillComp))
		return false;

	if (true == targetSkillComp->IsActivatedDefenceSkill())
		return false;

	return true;
}

FST_AISkill* USkillComponent::GetUseingSkill()
{
	return m_CurUsingSkill;
}

FST_AISkill* USkillComponent::GetUseingSkill(ACharacter* pChar)
{
	USkillComponent* targetSkillComp = pChar->FindComponentByClass<USkillComponent>();
	if (false == IsValid(targetSkillComp))
		return nullptr;

	return targetSkillComp->GetUseingSkill();
}

FST_SkillAnim* USkillComponent::GetSkillAnim(FName Row)
{
	if (nullptr == SkillAnimTable)
		return nullptr;

	FString context = FString::Printf(TEXT("%s not Find"), *Row.ToString());
	FST_SkillAnim* skillAnim = SkillAnimTable->FindRow<FST_SkillAnim>(Row, context);
	return skillAnim;
}

EN_BattleAnimState USkillComponent::CalcDefenseState()
{
	float DodgeRate = 100;
	float BigDodgeRate = 100;
	float BlockRate = 100;
	float BigBlockRate = 100;
	float HitRate = 100;
	float BigHitRate = 100;
	float SumRate = DodgeRate + BigDodgeRate + BlockRate + BigBlockRate + HitRate + BigHitRate;

	float randRate = m_RandStream.FRandRange(0.0f, SumRate);
	randRate -= DodgeRate;
	if (randRate < 0) return EN_BattleAnimState::Dodge;
	randRate -= BigDodgeRate;
	if (randRate < 0) return EN_BattleAnimState::BigDodge;
	randRate -= BlockRate;
	if (randRate < 0) return EN_BattleAnimState::Block;
	randRate -= BigBlockRate;
	if (randRate < 0) return EN_BattleAnimState::BigBlock;
	randRate -= HitRate;
	if (randRate < 0) return EN_BattleAnimState::Hit;

	return EN_BattleAnimState::BigHit;
}

void USkillComponent::OnEventMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (nullptr == m_CurUsingSkill)
		return;

	FString context = FString::Printf(TEXT("MontageEnded Skill Name = %s, bInterrupted = %d"), *m_CurUsingSkill->Name, bInterrupted);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, context);

	UCharStateComponent* charState = m_pOwnChar->FindComponentByClass<UCharStateComponent>();
	if (false == IsValid(charState))
		return;

	charState->UseCurSta(m_CurUsingSkill->StaminaUse);
	m_CurUsingSkill = nullptr;
}

void USkillComponent::OnEventBeginAttack_Implementation(ACharacter* pAttackChar)
{
	// Use Defence Skill
	FST_AISkill* pStUseingSkill = GetUseingSkill(pAttackChar);

	if (nullptr == pStUseingSkill)
		return;

	FST_SkillAnim* pSkillAnim = GetSkillAnim(pStUseingSkill->SkillAnimRow);
	if (nullptr == pSkillAnim)
		return;

	m_pOwnChar->StopAnimMontage();

	UAnimMontage* pDefenseStanceAnimMontage = pSkillAnim->GetSkillAnim(EN_BattleAnimState::DefenseStance);

	m_CurUsingSkill = nullptr;

	m_pOwnChar->PlayAnimMontage(pDefenseStanceAnimMontage);
}

void USkillComponent::OnEventBeforeHitNotify_Implementation(ACharacter* pAttackChar)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnEventHitNotify"));

	// Use Defence Skill
	FST_AISkill* pStUseingSkill = GetUseingSkill(pAttackChar);

	if (nullptr == pStUseingSkill)
		return;

	FST_SkillAnim* pSkillAnim = GetSkillAnim(pStUseingSkill->SkillAnimRow);
	if (nullptr == pSkillAnim)
		return;

	m_pOwnChar->StopAnimMontage();
	
	EN_BattleAnimState ResultAnimState = CalcDefenseState();
	UAnimMontage* pAnimMontage = pSkillAnim->GetSkillAnim(ResultAnimState);


	m_CurUsingSkill = nullptr;

	// FString context = FString::Printf(TEXT("Activated Defence Skill Name = %s"), *skill_InUse->Name);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, context);
	m_pOwnChar->PlayAnimMontage(pAnimMontage);
}

void USkillComponent::OnEventHitNotify_Implementation(ACharacter* pAttackChar)
{
}

void USkillComponent::SetSkill_AT(TArray<FName> names)
{
	if (IsValid(SkillTable))
	{
		for (FName iter : names)
		{
			// ContextString : 검색 실패시 출력 메시지
			FString context = FString::Printf(TEXT("%s not Find"), *iter.ToString());
			m_Skill_ATs.Push(SkillTable->FindRow<FST_AISkill>(iter, context));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SkillTable is Empty"));
		m_pOwnChar->Destroy();
		return;
	}

	if (false == IsValid(SkillAnimTable))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SkillAnimTable is Empty"));
		m_pOwnChar->Destroy();
		return;
	}
}

void USkillComponent::UseSkill()
{
	ACharacter* pTargetChar = GetTargetCharacter();
	if (false == IsValid(pTargetChar))
		return;

	// FindComponentByClass 는 Native 코드
	UCharStateComponent* charState = m_pOwnChar->FindComponentByClass<UCharStateComponent>();
	if (false == IsValid(charState))
		return;

	TArray<FST_AISkill*> filterSkills;

	for (FST_AISkill* skill : m_Skill_ATs)
	{
		if (skill->StaminaUse > charState->GetCurSta())
			continue;

		filterSkills.Add(skill);
	}

	if (filterSkills.IsEmpty())
		return;

	// pTargetChar->GetActorLocation();
	// FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(m_pOwnChar->GetActorLocation(), pTargetChar->GetActorLocation());
	// m_pOwnChar->SetActorRotation(LookAtRot);

	int32 randNum = m_RandStream.RandRange(0, filterSkills.Num() - 1);
	m_CurUsingSkill = filterSkills[randNum];
	// FString context = FString::Printf(TEXT("Activated Attack Skill Name = %s"), *skill_InUse->Name);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, context);
	FST_SkillAnim* pSkillAnim = GetSkillAnim(m_CurUsingSkill->SkillAnimRow);
	if (nullptr == pSkillAnim)
		return;

	m_pOwnChar->PlayAnimMontage(pSkillAnim->AttackAnim);
}

bool USkillComponent::IsActivatedDefenceSkill()
{
	return (m_CurUsingSkill->Type == EN_SkillType::Defence);
}

bool USkillComponent::IsActivatedAttackSkill()
{
	return (m_CurUsingSkill->Type == EN_SkillType::Attack);
}

ACharacter* USkillComponent::GetTargetCharacter()
{
	AAIBattleController* pCtrl = Cast<AAIBattleController>(m_pOwnChar->GetController());
	if (false == IsValid(pCtrl))
		return nullptr;

	ACharacter* pTargetChar = pCtrl->GetTargetChar();
	if (false == IsValid(pTargetChar))
		return nullptr;

	return pTargetChar;
}

