// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/AttackingHitNotify.h"
#include "Tools/SkillComponent.h"
#include "GameFramework/Character.h"

void UAttackingHitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ACharacter* pChar = Cast<ACharacter>(MeshComp->GetOwner());
	if (false == IsValid(pChar))
		return;

	// FindComponentByClass �� Native �ڵ�
	USkillComponent* skillcomp = pChar->FindComponentByClass<USkillComponent>();

	// GetComponentByClass �� �������Ʈ �ڵ�, ���ο� FindComponentByClass �����
	// USkillComponent* skillcomp = Cast< USkillComponent>(pChar->GetComponentByClass(USkillComponent::StaticClass()));

	if (false == IsValid(skillcomp))
		return;
	
	skillcomp->OnEventHitNotify();
}
