// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/CharStateComponent.h"

// Sets default values for this component's properties
UCharStateComponent::UCharStateComponent():m_CurSta(0), DeltaTime_RecoverPer(0)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bAutoActivate = true;
}


// Called when the game starts
void UCharStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UCharStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	UpdateStrRecover(DeltaTime);
}

void UCharStateComponent::UpdateStrRecover(float DeltaTime)
{
	if (GetCurSta() >= GetMaxSta())
		return;

	DeltaTime_RecoverPer += DeltaTime;

	if (DeltaTime_RecoverPer < 3)
		return;

	DeltaTime_RecoverPer -= 3;

	SetCurSta(GetCurSta() + GetStaRecoverPerSec());
}

bool UCharStateComponent::UseCurSta(float sta)
{
	bool OverSta = false;
	if (m_CurSta - sta < 0) OverSta = true;

	m_CurSta -= sta;

	m_CurSta = FMath::Clamp(m_CurSta, 0.0f, GetMaxSta());

	if (Event_UpdateSta.IsBound())
	{
		Event_UpdateSta.Broadcast(GetCurSta(), GetMaxSta());
	}

	return OverSta;
}

void UCharStateComponent::SetCurSta(float sta)
{
	m_CurSta = sta;
	
	if (Event_UpdateSta.IsBound())
	{
		Event_UpdateSta.Broadcast(GetCurSta(), GetMaxSta());
	}
}

