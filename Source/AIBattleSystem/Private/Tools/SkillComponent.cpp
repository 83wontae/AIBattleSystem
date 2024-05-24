// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/SkillComponent.h"
#include "GameMode/AIBattleSystemCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameMode/AIBattleSystemGameMode.h"

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
	AAIBattleController* pCtrl = Cast<AAIBattleController>(m_pOwnChar->GetController());
	if (IsValid(pCtrl))
	{
		m_pOwnChar->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &USkillComponent::OnEventMontageEnded);
	}

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
	if (IsValid(pCtrl->GetBlackboardComponent()) == false)
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

		if (m_pOwnChar->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() == true)
			break;

		if (m_Skill_ATs.IsEmpty() == true)
			break;

		int32 randNum = m_Stream.RandRange(0, m_Skill_ATs.Num() - 1);
		m_pOwnChar->PlayAnimMontage(m_Skill_ATs[randNum]->Anim);

	}	break;
	default: {

	}
	}
}

void USkillComponent::OnEventMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}

void USkillComponent::OnEventHitNotify_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnEventHitNotify"));

	// Defence Skill
	if (m_Skill_DFs.IsEmpty() == true)
		return;

	int32 randNum = m_Stream.RandRange(0, m_Skill_DFs.Num() - 1);
	m_pOwnChar->PlayAnimMontage(m_Skill_DFs[randNum]->Anim);
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

