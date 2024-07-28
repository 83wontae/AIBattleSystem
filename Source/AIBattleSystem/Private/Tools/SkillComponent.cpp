// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/SkillComponent.h"
#include "GameMode/AIBattleSystemCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameMode/AIBattleSystemGameMode.h"
#include "Tools/CharStateComponent.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent()
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

	SetSkill_AT(Skill_AT_Names);

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

	// Bind MontageEnded Event
	m_pOwnChar->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &USkillComponent::OnEventMontageEnded);

	// Set AI Random Seed 
	AAIBattleSystemGameMode* pGM = Cast<AAIBattleSystemGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(pGM))
	{
		int32 seed = pGM->GetSeed();
		m_Stream.Initialize(seed);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("AI Seed = %d"), seed));
	}
	else
	{
		m_Stream.Initialize(0);
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

	m_AI_State = (EN_AIState)pCtrl->GetBlackboardComponent()->GetValueAsEnum("AiState");
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Aistate = %d"), aistate));
	switch (m_AI_State)
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

bool USkillComponent::CheckUseDefenceSkill()
{
	// 상대방이 회피중인지 채크
	AAIBattleController* pCtrl = Cast<AAIBattleController>(m_pOwnChar->GetController());
	if (false == IsValid(pCtrl))
		return false;

	ACharacter* pTargetChar = pCtrl->GetTargetChar();
	if (false == IsValid(pTargetChar))
		return false;

	USkillComponent* targetState = pTargetChar->FindComponentByClass<USkillComponent>();
	if (false == IsValid(targetState))
		return false;

	if (true == targetState->IsActivatedDefenceSkill())
		return false;

	return true;
}

void USkillComponent::OnEventMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (nullptr == skill_InUse)
		return;

	FString context = FString::Printf(TEXT("MontageEnded Skill Name = %s, bInterrupted = %d"), *skill_InUse->Name, bInterrupted);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, context);

	UCharStateComponent* charState = m_pOwnChar->FindComponentByClass<UCharStateComponent>();
	if (false == IsValid(charState))
		return;

	charState->UseCurSta(skill_InUse->StaminaUse);
	skill_InUse = nullptr;
}

void USkillComponent::OnEventBeginAttack_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnEventHitNotify"));

	// 상대방이 회피중인지 채크
	if (false == CheckUseDefenceSkill())
		return;

	// Use Defence Skill
	UCharStateComponent* charState = m_pOwnChar->FindComponentByClass<UCharStateComponent>();
	if (false == IsValid(charState))
		return;

	TArray<FST_AISkill*> filterSkills;

	for (FST_AISkill* skill : m_Skill_DFs)
	{
		if (skill->StaminaUse > charState->GetCurSta())
			continue;

		filterSkills.Add(skill);
	}

	if (true == filterSkills.IsEmpty())
		return;

	m_pOwnChar->StopAnimMontage();

	int32 randNum = m_Stream.RandRange(0, filterSkills.Num() - 1);
	skill_InUse = filterSkills[randNum];
	// FString context = FString::Printf(TEXT("Activated Defence Skill Name = %s"), *skill_InUse->Name);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, context);
	m_pOwnChar->PlayAnimMontage(skill_InUse->Anim);
}

void USkillComponent::OnEventHitNotify_Implementation()
{
	/*
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnEventHitNotify"));

	// Defence Skill
	UCharStateComponent* charState = m_pOwnChar->FindComponentByClass<UCharStateComponent>();
	if (false == IsValid(charState))
		return;

	TArray<FST_AISkill*> filterSkills;

	for (FST_AISkill* skill : m_Skill_DFs)
	{
		if (skill->StaminaUse > charState->GetCurSta())
			continue;

		filterSkills.Add(skill);
	}

	if (true == filterSkills.IsEmpty())
		return;

	m_pOwnChar->StopAnimMontage();

	int32 randNum = m_Stream.RandRange(0, filterSkills.Num() - 1);
	skill_InUse = filterSkills[randNum];
	// FString context = FString::Printf(TEXT("Activated Defence Skill Name = %s"), *skill_InUse->Name);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, context);
	m_pOwnChar->PlayAnimMontage(skill_InUse->Anim);
	*/
}

void USkillComponent::SetSkill_AT(TArray<FName> names)
{
	if (IsValid(AT_Table))
	{
		for (FName iter : names)
		{
			// ContextString : 검색 실패시 출력 메시지
			FString context = FString::Printf(TEXT("%s not Find"), *iter.ToString());
			m_Skill_ATs.Push(AT_Table->FindRow<FST_AISkill>(iter, context));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AT_Table is Empty"));
	}
}

void USkillComponent::UseSkill()
{
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

	int32 randNum = m_Stream.RandRange(0, filterSkills.Num() - 1);
	skill_InUse = filterSkills[randNum];
	// FString context = FString::Printf(TEXT("Activated Attack Skill Name = %s"), *skill_InUse->Name);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, context);
	m_pOwnChar->PlayAnimMontage(skill_InUse->Anim);
}

bool USkillComponent::IsActivatedDefenceSkill()
{
	return (skill_InUse->Type == EN_SkillType::Defence);
}

bool USkillComponent::IsActivatedAttackSkill()
{
	return (skill_InUse->Type == EN_SkillType::Attack);
}

