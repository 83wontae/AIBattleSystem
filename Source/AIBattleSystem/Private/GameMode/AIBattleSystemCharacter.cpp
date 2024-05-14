// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode/AIBattleSystemCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameMode/AIBattleController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/AIBattleSystemGameMode.h"


//////////////////////////////////////////////////////////////////////////
// AAIBattleSystemCharacter

AAIBattleSystemCharacter::AAIBattleSystemCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	m_AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSource"));

	// Add the Sight stimulus to the AIPerceptionStimuliSourceComponent
	m_AIPerceptionStimuliSource->bAutoRegister = true;
	m_AIPerceptionStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
}

void AAIBattleSystemCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	AAIBattleController* pCtrl = Cast<AAIBattleController>(GetController());
	if (IsValid(pCtrl))
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AAIBattleSystemCharacter::OnEventMontageEnded);
	}
}

void AAIBattleSystemCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AAIBattleController* pCtrl = Cast<AAIBattleController>(GetController());
	if (IsValid(pCtrl))
	{
		TickAI(pCtrl, DeltaSeconds);
	}
}

void AAIBattleSystemCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Set Generic TeamId
	TeamId = m_ID;

	AAIBattleController* pCtrl = Cast<AAIBattleController>(GetController());
	if (IsValid(pCtrl))
	{
		// Set Generic TeamId
		pCtrl->TeamId = m_ID;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s TeamId = %d"), *pCtrl->GetName(), pCtrl->TeamId));

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
}

void AAIBattleSystemCharacter::TickAI(AAIBattleController* pCtrl, float DeltaSeconds)
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

		if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() == true)
			break;

		if (m_Skills.IsEmpty() == true)
			break;

		int32 randNum = m_Stream.RandRange(0, m_Skills.Num() - 1);
		PlayAnimMontage(m_Skills[randNum].Anim);

	}	break;
	default: {

	}}
}

void AAIBattleSystemCharacter::OnEventMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAIBattleSystemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAIBattleSystemCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAIBattleSystemCharacter::Look);

	}

}

void AAIBattleSystemCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAIBattleSystemCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}




