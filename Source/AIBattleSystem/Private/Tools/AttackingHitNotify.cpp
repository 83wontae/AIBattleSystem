// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/AttackingHitNotify.h"
#include "GameMode/AIBattleSystemCharacter.h"

void UAttackingHitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AAIBattleSystemCharacter* pChar = Cast<AAIBattleSystemCharacter>(MeshComp->GetOwner());
	if (false == IsValid(pChar))
		return;

	pChar->OnEventHitNotify();
}
