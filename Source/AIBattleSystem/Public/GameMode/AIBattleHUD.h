// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AIBattleHUD.generated.h"

/**
 * 
 */
UCLASS()
class AIBATTLESYSTEM_API AAIBattleHUD : public AHUD
{
	GENERATED_BODY()

public:
	// To add mapping context
	virtual void BeginPlay();

public:
	// TSubclassOf : ������ Ŭ������ Ŭ������ ����� ��� Ŭ������ ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> HudWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* HudWidget;
};
