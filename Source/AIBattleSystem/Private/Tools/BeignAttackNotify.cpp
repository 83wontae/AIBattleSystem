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

	// FindComponentByClass �� Native �ڵ�
	USkillComponent* skillcomp = pTargetChar->FindComponentByClass<USkillComponent>();

	// GetComponentByClass �� �������Ʈ �ڵ�, ���ο� FindComponentByClass �����
	// USkillComponent* skillcomp = Cast< USkillComponent>(pTargetChar->GetComponentByClass(USkillComponent::StaticClass()));

	if (false == IsValid(skillcomp))
		return;

	skillcomp->OnEventBeginAttack();
}
