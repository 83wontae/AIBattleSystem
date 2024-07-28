// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/BeignAttackNotify.h"
#include "Tools/SkillComponent.h"
#include "GameFramework/Character.h"

void UBeignAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ACharacter* pChar = Cast<ACharacter>(MeshComp->GetOwner());
	if (false == IsValid(pChar))
		return;

	AAIBattleController* pCtrl = Cast<AAIBattleController>(pChar->GetController());
	if (false == IsValid(pCtrl))
		return;

	ACharacter* pTargetChar = pCtrl->GetTargetChar();
	if (false == IsValid(pTargetChar))
		return;

	// FindComponentByClass 는 Native 코드
	USkillComponent* skillcomp = pTargetChar->FindComponentByClass<USkillComponent>();

	// GetComponentByClass 는 블루프린트 코드, 내부에 FindComponentByClass 사용함
	// USkillComponent* skillcomp = Cast< USkillComponent>(pTargetChar->GetComponentByClass(USkillComponent::StaticClass()));

	if (false == IsValid(skillcomp))
		return;

	skillcomp->OnEventBeginAttack();
}
