// Copyright Epic Games, Inc. All Rights Reserved.

#include "FocusfireCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetHealth.h"
#include "FocusBase.h"
#include "GameplayAbility_FocusDash.h"
#include "GameplayAbility_FocusPeriod.h"
#include "GameplayAbility_FocusShoot.h"
#include "GameplayTagsManager.h"
#include "KismetTraceUtils.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AFocusfireCharacter::AFocusfireCharacter()
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
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	c_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	c_CameraBoom->SetupAttachment(RootComponent);
	c_CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	c_CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow (third person) camera
	c_ThirdPOVCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	c_ThirdPOVCamera->SetupAttachment(c_CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	c_ThirdPOVCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	c_ThirdPOVCamera->bAutoActivate = true; // Default camera
	// Create arrow component to "save the place" of the third person camera
	c_ThirdPOVArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ThirdPersonCameraArrow"));
	c_ThirdPOVArrow->SetupAttachment(c_CameraBoom, USpringArmComponent::SocketName);
	c_ThirdPOVArrow->SetRelativeTransform(c_ThirdPOVCamera->GetRelativeTransform());

	// Create a first person camera
	c_FirstPOVCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	c_FirstPOVCamera->SetupAttachment(GetMesh(), "head");
	c_FirstPOVCamera->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	c_FirstPOVCamera->SetRelativeLocation(FVector(0.0f, 10.0f, 0.0f));
	c_FirstPOVCamera->bUsePawnControlRotation = true; // Rotate camera with look
	c_FirstPOVCamera->bAutoActivate = false;
	// Create arrow component to "save the place" of the first person camera
	c_FirstPOVArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FirstPersonCameraArrow"));
	c_FirstPOVArrow->SetupAttachment(GetMesh(), "head");
	c_FirstPOVArrow->SetRelativeTransform(c_FirstPOVCamera->GetRelativeTransform());

	// Set the "current camera", as only the third person camera is auto-activated
	CurrentCamera = c_ThirdPOVCamera;

	// Create arrow component to act as the spawn point for where FocusBase are shot out from
	c_FocusShootArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FocusBaseSpawnArrow"));
	c_FocusShootArrow->SetupAttachment(GetMesh(), "head");
	c_FocusShootArrow->SetRelativeTransform(c_FirstPOVCamera->GetRelativeTransform());
	
	// Create and initialize the AbilitySystemComponent
	c_AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// Create an initialize AttributeSets
	as_HealthAttributeSet = CreateDefaultSubobject<UAttributeSetHealth>(TEXT("HealthAttributeSet"));
}

void AFocusfireCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (c_AbilitySystemComponent)
	{
		c_AbilitySystemComponent->InitAbilityActorInfo(this, this);
		c_AbilitySystemComponent->SetNumericAttributeBase(UAttributeSetHealth::GetHealthAttribute(), 100);
		c_AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &AFocusfireCharacter::OnGameplayAbilityEnded);
	}
	if (as_HealthAttributeSet)
	{
		as_HealthAttributeSet->OnHealthChanged.AddDynamic(this, &AFocusfireCharacter::HandleHealthChanged);
	}
}

void AFocusfireCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AFocusfireCharacter::OnTickRaycastForFocus();
}

//////////////////////////////////////////////////////////////////////////
// Input
void AFocusfireCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AFocusfireCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::Look);

		// Using focus ability
		EnhancedInputComponent->BindAction(UseFocusAbilityAction, ETriggerEvent::Started, this, &AFocusfireCharacter::UseFocusAbility);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFocusfireCharacter::Move(const FInputActionValue& Value)
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

void AFocusfireCharacter::Look(const FInputActionValue& Value)
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

void AFocusfireCharacter::UseFocusAbility(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("ccc Using Focusfire Ability"));
	//c_AbilitySystemComponent->TryActivateAbilityByClass(UGameplayAbility_FocusShoot::StaticClass());
}

// END Input
//////////////////////////////////////////////////////////////////////////


FVector AFocusfireCharacter::LerpCurrentCameraLocation(const float Alpha)
{
	FVector _a_location = (CurrentCamera == c_FirstPOVCamera) ? c_FirstPOVArrow->GetComponentLocation() : c_ThirdPOVArrow->GetComponentLocation();
	FVector _b_location = (CurrentCamera == c_FirstPOVCamera) ? c_ThirdPOVArrow->GetComponentLocation() : c_FirstPOVArrow->GetComponentLocation();
	return FMath::Lerp(_a_location, _b_location, Alpha);
}

void AFocusfireCharacter::SwitchCameraBegin()
{
	if (CurrentCamera == c_FirstPOVCamera)
	{
		bUseControllerRotationYaw = false; // Do NOT turn Player with camera in third person view
	}
	else if (CurrentCamera == c_ThirdPOVCamera)
	{
		// Set bUseControllerRotationYaw = true on SwitchCameraEnd(), so that DURING transition, blueprint can lerp Player yaw rotation towards camera dir
	}
	flag_isCurrentlySwitchingCamera = true;
}

void AFocusfireCharacter::SwitchCameraEnd()
{
	if (CurrentCamera == c_FirstPOVCamera)
	{
		CurrentCamera->SetWorldLocation(c_FirstPOVArrow->GetComponentLocation()); // Reset moved first person camera back to default arrow pos
		CurrentCamera->SetActive(false);
		CurrentCamera = c_ThirdPOVCamera;
		CurrentCamera->SetActive(true);
		bUseControllerRotationYaw = false; // Do NOT turn Player with camera
	}
	else if (CurrentCamera == c_ThirdPOVCamera)
	{
		CurrentCamera->SetWorldLocation(c_ThirdPOVArrow->GetComponentLocation()); // Reset moved third person camera back to default arrow pos
		CurrentCamera->SetActive(false);
		CurrentCamera = c_FirstPOVCamera;
		CurrentCamera->SetActive(true);
		bUseControllerRotationYaw = true; // Turn Player with camera
	}
	CurrentCamera->SetActive(true);
	flag_isCurrentlySwitchingCamera = false;
}

void AFocusfireCharacter::OnTickRaycastForFocus()
{
	// If Player is not currently have "GameplayAbility.Focus.Period" active, then return
	if (not (c_AbilitySystemComponent and c_AbilitySystemComponent->HasMatchingGameplayTag(UGameplayTagsManager::Get().RequestGameplayTag("GameplayAbility.Focus.Period"))))
	{
		return;
	}
	
	const FVector _TraceStart = GetCurrentCamera()->GetComponentLocation();
	const FVector _TraceEnd = _TraceStart + GetCurrentCamera()->GetForwardVector() * RangeOfFocusRaycast;
	const TArray<AActor*> _ActorsToIgnore = {GetOwner()};
	const FColor _ColorBeforeHit = FColor::Green;
	const FColor _ColorAfterHit = FColor::Red;

	FHitResult _HitResult;
	if (UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		_TraceStart,
		_TraceEnd,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		_ActorsToIgnore,
		EDrawDebugTrace::ForDuration, // Set to EDrawDebugTrace::None if not debugging
		_HitResult,
		true,
		_ColorBeforeHit,
		_ColorAfterHit
	))
	{
		if (AFocusBase* _FocusBase = Cast<AFocusBase>(_HitResult.GetActor()))
		{
			UE_LOG(LogTemp, Warning, TEXT("ccc Line trace has hit: %s"), *(_FocusBase->GetName()));
			CurrentFocusInRange = _FocusBase;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ccc Line trace has hit: %s"), *(_HitResult.GetActor()->GetName()));
			CurrentFocusInRange = nullptr;
		}
	}
	else
	{
		CurrentFocusInRange = nullptr;
	}
	OnFocusInRangeChanged.Broadcast(CurrentFocusInRange);
}

void AFocusfireCharacter::OnGameplayAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (UGameplayAbility_FocusDash* _endedDash = Cast<UGameplayAbility_FocusDash>(AbilityEndedData.AbilityThatEnded))
	{
		OnFocusDashEnded(AbilityEndedData.bWasCancelled);
	}
	else if (UGameplayAbility_FocusPeriod* _endedPeriod = Cast<UGameplayAbility_FocusPeriod>(AbilityEndedData.AbilityThatEnded))
	{
		OnFocusPeriodEnded(AbilityEndedData.bWasCancelled);
	}
}

void AFocusfireCharacter::HandleHealthChanged(float Magnitude, float NewHealth)
{
	if (NewHealth <= 0)
	{
		Destroy();
	}
}
