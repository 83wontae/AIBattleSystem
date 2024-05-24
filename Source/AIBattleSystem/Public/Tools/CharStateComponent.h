// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharStateComponent.generated.h"

// struct 이름 앞에 반드시 F가 붙어야함
USTRUCT(BlueprintType)
struct FST_CharState : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FST_CharState()
		: Strength(0)
		, Dexterity(0)
		, Intelligence(0)
		, Stamina(0)
		, Charisma(0)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateData", meta=(ClampMin = "0"))
	int32 Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateData", meta=(ClampMin = "0"))
	int32 Dexterity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateData", meta=(ClampMin = "0"))
	int32 Intelligence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateData", meta=(ClampMin = "0"))
	int32 Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StateData", meta=(ClampMin = "0"))
	int32 Charisma;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIBATTLESYSTEM_API UCharStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharStateComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FST_CharState m_BaseState;

	UFUNCTION(BlueprintCallable)
	void SetBaseState(FST_CharState state) { 
		m_BaseState = state;
		InitState();
	};

	void InitState() {
		m_CurSta = m_BaseState.Stamina;
	};

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FST_CharState GetBaseState() { return m_BaseState; };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
	float m_CurSta;

};
