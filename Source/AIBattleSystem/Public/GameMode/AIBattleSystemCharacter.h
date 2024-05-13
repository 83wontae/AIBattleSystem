// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIBattleController.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AIBattleSystemCharacter.generated.h"


UCLASS(config=Game)
class AAIBattleSystemCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	AAIBattleSystemCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay() override;

	/**
	 *	Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame.
	 *	Note that Tick is disabled by default, and you will need to check PrimaryActorTick.bCanEverTick is set to true to enable it.
	 *
	 *	@param	DeltaSeconds	Game time elapsed during last frame modified by the time dilation
	 */
	virtual void Tick(float DeltaSeconds) override;

	/**
	 * Called when this Pawn is possessed. Only called on the server (or in standalone).
	 * @param NewController The controller possessing this pawn
	 */
	virtual void PossessedBy(AController* NewController) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	//---[GenericTeamAgentInterface Area]---//
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", Meta = (ExposeOnSpawn = "true"))
	int32 m_ID;

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

protected:
	FGenericTeamId TeamId;

	//---[AI Skill & Move Area]---//
public:
	void TickAI(class AAIBattleController* pCtrl, float DeltaSeconds);

	UFUNCTION()
	void OnEventMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	UPROPERTY(EditAnywhere)
	class UAIPerceptionStimuliSourceComponent* m_AIPerceptionStimuliSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = "true"))
	TArray<FST_AISkill> m_Skills;

	FRandomStream m_Stream;

	UPROPERTY(BlueprintReadOnly)
	EN_AIState m_AI_State;
};

