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
#include "ActorComponent_ManagerFocus.h"
#include "AttributeSetHealth.h"
#include "FocusBase.h"
#include "FocusfireGameState.h"
#include "GameplayAbility_DodgeRoll.h"
#include "GameplayAbility_FocusDash.h"
#include "GameplayAbility_FocusPeriod.h"
#include "GameplayAbility_FocusShoot.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagsManager.h"
#include "KismetTraceUtils.h"
#include "PingSphere.h"
#include "UserWidget_FocusSelector.h"
#include "UserWidget_PlayerHUD.h"
#include "Blueprint/UserWidget.h"

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
	GetCharacterMovement()->GravityScale = DefaultGravityScale;

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
	AttributeSet_HealthAttributeSet = CreateDefaultSubobject<UAttributeSetHealth>(TEXT("HealthAttributeSet"));
}

void AFocusfireCharacter::BeginPlay()
{
	Super::BeginPlay();

	FirstPersonPOVCameraOffset = GetActorLocation() - GetFirstPOVCamera()->GetComponentLocation();

	// Set up Ability System Component
	if (c_AbilitySystemComponent)
	{
		c_AbilitySystemComponent->InitAbilityActorInfo(this, this);
		c_AbilitySystemComponent->AbilityActivatedCallbacks.AddUObject(this, &AFocusfireCharacter::OnGameplayAbilityStarted);
		c_AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &AFocusfireCharacter::OnGameplayAbilityEnded);

		if (AttributeSet_HealthAttributeSet)
		{
			AttributeSet_HealthAttributeSet = c_AbilitySystemComponent->GetSet<UAttributeSetHealth>();
			c_AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet_HealthAttributeSet->GetHealthAttribute()).AddUObject(this, &AFocusfireCharacter::HandleHealthChanged);
		}
	}

	// Set up Focus Selector (radial menu)
	if (FocusSelectorWidgetClass)
	{
		FocusSelectorWidget = CreateWidget<UUserWidget_FocusSelector>(GetWorld(), FocusSelectorWidgetClass);

		// Add all the types of FocusBase to the radial selector menu
		for (TSubclassOf<AFocusBase> _focusType : EquippedFocuses)
		{
			FocusSelectorWidget->AddTypeOfFocus(_focusType);
		}
	}

	// Set up Player HUD
	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UUserWidget_PlayerHUD>(GetWorld(), PlayerHUDClass);
		PlayerHUDWidget->AddToViewport();
		PlayerHUDWidget->initializePlayerHUD(this);

		// Set starting FocusBase to shoot
		const FString _FocusName = GetWorld()->GetGameState<AFocusfireGameState>()->GetManagerFocus()->GetFocusString(TypeOfFocusToShoot);
		PlayerHUDWidget->OnCurrentlySelectedFocusChanged(_FocusName);
	}
}

void AFocusfireCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AFocusfireCharacter::OnTickRaycastForDashableToFocus();

	// If Player is currently locked onto a FocusBase, then pivot Player/camera around it
	if (IsValid(CurrentLockedOnFocus))
	{
		AFocusfireCharacter::PivotAroundLockedFocus();
	}
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
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFocusfireCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::Look);

		// Using focus ability
		EnhancedInputComponent->BindAction(UseFocusAbilityAction, ETriggerEvent::Started, this, &AFocusfireCharacter::UseFocusAbility);

		// Cancel a "locked-on" FocusBase during "GameplayAbility.Focus.Period"
		EnhancedInputComponent->BindAction(CancelLockedFocusAction, ETriggerEvent::Started, this, &AFocusfireCharacter::DoSecondaryAction);

		// Toggle the FocusBase selector radial menu
		EnhancedInputComponent->BindAction(FocusSelectorAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::ToggleFocusSelector);

		// Ping
		EnhancedInputComponent->BindAction(PingAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::DoSecondaryAction);

		// Cancel Ping
		EnhancedInputComponent->BindAction(CancelPingAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::CancelPing);
		
		// Ping distance adjustment
		EnhancedInputComponent->BindAction(AdjustPingDistanceAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::AdjustPingDistance);

		// Dodge roll
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AFocusfireCharacter::Dodge);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AFocusfireCharacter::Sprint);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFocusfireCharacter::StartJump(const FInputActionValue& Value)
{
	OnInputJump();
}

void AFocusfireCharacter::Move(const FInputActionValue& Value)
{
	// Do nothing if Player has tag to disable movement input
	if (c_AbilitySystemComponent->HasMatchingGameplayTag(DisableMovementInputTag))
	{
		return;
	}
	
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (
			Controller != nullptr
		&&
			CurrentLockedOnFocus == nullptr // Only allow move if not currently "locked-on" to a FocusBase
	)
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
	if (not c_AbilitySystemComponent->HasMatchingGameplayTag(DuringFocusPeriodTag))
	{
		return; // NOT in GameplayAbility.Focus.Period state, so do nothing
	}

	// Use Ability of currently "locked on" FocusBase
	if (CurrentLockedOnFocus != nullptr && CurrentLockedOnFocus == CurrentDashableToFocus)
	{
		OnInputFocusBaseActivateAbility(CurrentLockedOnFocus);
		UE_LOG(LogTemp, Warning, TEXT("ccc Use FocusBase.GameplayAbility"));
		return;
	}

	// Use "GameplayAbility.Focus.Dash" onto the FocusBase under the crosshair
	if (CurrentDashableToFocus != nullptr)
	{
		OnInputFocusDash();
		UE_LOG(LogTemp, Warning, TEXT("ccc Use GameplayAbility.Focus.Dash"));
		return;
	}

	// Use "GameplayAbility.Focus.Shoot" to shoot out the currently selected FocusBase in viewing direction (nothing under crosshair)
	if (CurrentDashableToFocus == nullptr)
	{
		OnInputFocusShoot();
		UE_LOG(LogTemp, Warning, TEXT("ccc Use GameplayAbility.Focus.Shoot"));
		return;
	}
}

void AFocusfireCharacter::CancelLockedFocus(const FInputActionValue& Value)
{
	// Comment because check in DoSecondaryAction()
	// if (not c_AbilitySystemComponent->HasMatchingGameplayTag(DuringFocusPeriodTag))
	// {
	// 	return; // NOT in GameplayAbility.Focus.Period state, so do nothing
	// }

	// Unlock the "locked-on" FocusBase, to prevent pivoting around it anymore
	CurrentLockedOnFocus = nullptr;
	OnInputFocusPeriodCancelLocked(); // Will signal BP to "input cancel" the "GameplayAbility.Focus.Period", which will re-slowdown the Player

	// Modify HUD to display elements before locked focus
	PlayerHUDWidget->ToggleFocusLockedHUDElements(false);
}

void AFocusfireCharacter::DoSecondaryAction(const FInputActionValue& Value)
{
	// Only Ping if in first person POV
	if (not c_AbilitySystemComponent->HasMatchingGameplayTag(DuringFocusPeriodTag))
	{
		return; // NOT in GameplayAbility.Focus.Period state, so do nothing
	}

	if (CurrentDashableToFocus != nullptr)
	{
		CancelLockedFocus(Value);
	}
	else
	{
		DoPing(Value);
	}
}

void AFocusfireCharacter::ToggleFocusSelector(const FInputActionValue& Value)
{
	// If radial menu widget was never created, return
	if (not IsValid(FocusSelectorWidget))
		return;
	
	if (Value.Get<bool>()) // On Press
	{
		if (not FocusSelectorWidget->IsInViewport())
		{
			FocusSelectorWidget->AddToViewport();
			GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameAndUI());

			// Set mouse position to CENTER of the screen
			int _viewX;
			int _viewY;
			GetWorld()->GetFirstPlayerController()->GetViewportSize(_viewX, _viewY);
			GetWorld()->GetFirstPlayerController()->SetMouseLocation(_viewX / 2, _viewY / 2);
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
		}
	}
	else // On Release
	{
		if (FocusSelectorWidget->IsInViewport())
		{
			// Set the current FocusBase to shoot, only if properly selected
			TSubclassOf<AFocusBase> _selectedFocus = FocusSelectorWidget->GetSelectedFocus();
			if (_selectedFocus != nullptr)
			{
				TypeOfFocusToShoot = _selectedFocus;

				// Reflect the currently selected FocusBase type on HUD
				if (IsValid(PlayerHUDWidget))
				{
					const FString _FocusName = GetWorld()->GetGameState<AFocusfireGameState>()->GetManagerFocus()->GetFocusString(TypeOfFocusToShoot);
					PlayerHUDWidget->OnCurrentlySelectedFocusChanged(_FocusName);
				}
			}
			
			FocusSelectorWidget->RemoveFromParent();
			GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
			GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
		}
	}
}

void AFocusfireCharacter::DoPing(const FInputActionValue& Value)
{
	// Comment because check in DoSecondaryAction()
	// // Only Ping if in first person POV
	// if (not c_AbilitySystemComponent->HasMatchingGameplayTag(DuringFocusPeriodTag))
	// {
	// 	return; // NOT in GameplayAbility.Focus.Period state, so do nothing
	// }

	if (Value.Get<bool>()) // On Press
	{
		OnInputPing(EPingInput::PROSPECTIVE_PING); // Spawn prospective ping
	}
	else // On Release
	{
		OnInputPing(EPingInput::CONFIRM_PING); // Confirm ping
	}
}

void AFocusfireCharacter::CancelPing(const FInputActionValue& Value)
{
	// Only Cancel prospective ping if during GameplayAbility.Ping
	if (c_AbilitySystemComponent->HasMatchingGameplayTag(DuringPingTag))
	{
		OnInputPing(EPingInput::CANCEL_PING);
	}
}

void AFocusfireCharacter::AdjustPingDistance(const FInputActionValue& Value)
{
	float _adjustmentDirection = Value.Get<float>();
	OnInputAdjustPingDistance(_adjustmentDirection);
}

void AFocusfireCharacter::Dodge(const FInputActionValue& Value)
{
	// Do nothing if Player has tag to disable movement input
	if (c_AbilitySystemComponent->HasMatchingGameplayTag(DisableMovementInputTag))
	{
		return;
	}
	
	OnInputDodgeRoll(); // Signal BP
}

void AFocusfireCharacter::Sprint(const FInputActionValue& Value)
{
	if (Value.Get<bool>()) // On Press
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * 1.6;
	}
	else // On Release
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	}
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

void AFocusfireCharacter::OnTickRaycastForDashableToFocus()
{
	// If Player does not currently have "GameplayAbility.Focus.Period" active, then return
	if (not (c_AbilitySystemComponent and c_AbilitySystemComponent->HasMatchingGameplayTag(DuringFocusPeriodTag)))
	{
		return;
	}

	// Begin with potential FocusBase set as nullptr
	AFocusBase* _temp_DashableToFocus = nullptr;
	APingSphere* _temp_PingUnderCrosshair = nullptr;

	// Raycasts
	if (AActor* _hitActor = RaycastForFocusOrPing(true)) // First attempt to hit a FocusBase (because FocusBase takes priority over PingSphere)
	{
		_temp_DashableToFocus = Cast<AFocusBase>(_hitActor);
	}
	else
	{
		_hitActor = RaycastForFocusOrPing(false); // If it fails, then attempt to hit a PingSphere
		_temp_PingUnderCrosshair = Cast<APingSphere>(_hitActor);
	}

	// Only notify if raycast target changed from last frame
	if (CurrentPingUnderCrosshair != _temp_PingUnderCrosshair) // Ping
    {
		CurrentPingUnderCrosshair = _temp_PingUnderCrosshair;

		// Change HUD
		PlayerHUDWidget->ToggleShootToLockElements(CurrentPingUnderCrosshair != nullptr);
    }
	if (CurrentDashableToFocus != _temp_DashableToFocus) // Dash-able FocusBase
	{
		CurrentDashableToFocus = _temp_DashableToFocus;
		
		// Change HUD
		PlayerHUDWidget->ToggleFocusDashHUDElements(CurrentDashableToFocus != nullptr);
	}

	// Update Crosshair per tick
	OnDashableToFocusChanged.Broadcast(CurrentDashableToFocus);
}

AActor* AFocusfireCharacter::RaycastForFocusOrPing(bool bQueryFocusOnly)
{
	const FVector _TraceStart = GetCurrentCamera()->GetComponentLocation();
	const FVector _TraceEnd = _TraceStart + GetCurrentCamera()->GetForwardVector() * RangeOfDashableToFocusRaycast * 2;
	const TArray<AActor*> _ActorsToIgnore = {GetOwner()};
	const FColor _ColorBeforeHit = FColor::Green;
	const FColor _ColorAfterHit = FColor::Red;
	ETraceTypeQuery _TraceType = bQueryFocusOnly ? UEngineTypes::ConvertToTraceType(ECC_Focus) : UEngineTypes::ConvertToTraceType(ECC_Ping);
	FHitResult _HitResult;
	if (UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		_TraceStart,
		_TraceEnd,
		_TraceType,
		false,
		_ActorsToIgnore,
		EDrawDebugTrace::None, // Set to EDrawDebugTrace::None if not debugging, otherwise EDrawDebugTrace::ForDuration
		_HitResult,
		true,
		_ColorBeforeHit,
		_ColorAfterHit
	))
	{
		if (bQueryFocusOnly)
		{
			// Check if FocusBase is in dash range
			if (_HitResult.Distance <= RangeOfDashableToFocusRaycast)
			{
				if (AFocusBase* _FocusBase = Cast<AFocusBase>(_HitResult.GetActor()))
				{
					if (_FocusBase->GetCanBeInteractedWith())
					{
						return _FocusBase;
					}
				}
			}
		}

		// Only attempt to hit PingSphere if NOT querying for FocusBase
		else
		{
			if (APingSphere* _PingSphere = Cast<APingSphere>(_HitResult.GetActor()))
			{
				return _PingSphere;
			}
		}
	}
	
	return nullptr;
}

void AFocusfireCharacter::PivotAroundLockedFocus()
{
	// Return if there is no "locked-on" FocusBase
	if (not IsValid(CurrentLockedOnFocus))
		return;

	// Set the position of the Player to pivot around the "locked-on" FocusBase while looking with the first-person POV camera
	const FVector _nextPosition = GetPlayerPivotPosAroundLockedFocus(CurrentLockedOnFocus, LockedOnFocusDistance);
	SetActorLocation(_nextPosition);
}

void AFocusfireCharacter::OnGameplayAbilityStarted(UGameplayAbility* Ability)
{
	// Disable Player input on gameplay ability start
	if (c_AbilitySystemComponent->HasMatchingGameplayTag(DisableAllInputTag))
	{
		APlayerController* _PlayerController = GetWorld()->GetFirstPlayerController();
		DisableInput(_PlayerController);
	}
}

void AFocusfireCharacter::OnGameplayAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	// If Player no longer has the tag to disable input, re-enable it
	if (not c_AbilitySystemComponent->HasMatchingGameplayTag(DisableAllInputTag))
	{
		APlayerController* _PlayerController = GetWorld()->GetFirstPlayerController();
		EnableInput(_PlayerController);
	}

	// If Player has the jump key buffered, then immediately attempt to jump
	if (c_AbilitySystemComponent->HasMatchingGameplayTag(InputBuffer_Jump_Tag))
	{
		// Immediately jump after "GameplayAbility.DodgeRoll"
		if (UGameplayAbility_DodgeRoll* _endedDodgeRoll = Cast<UGameplayAbility_DodgeRoll>(AbilityEndedData.AbilityThatEnded))
		{
			OnInputJump(false);
		}
	}
	
	if (UGameplayAbility_FocusDash* _endedDash = Cast<UGameplayAbility_FocusDash>(AbilityEndedData.AbilityThatEnded))
	{
		// Lock on to the FocusBase that was just dashed to (allows use of its ability)
		CurrentLockedOnFocus = CurrentDashableToFocus;
		LockedOnFocusDistance = (CurrentLockedOnFocus->GetActorLocation() - GetActorLocation()).Length();
		PivotAroundLockedFocus();

		// Toggle HUD
		PlayerHUDWidget->ToggleFocusLockedHUDElements(true);

		// BP handles activating "GameplayAbility.Focus.Period" right after
		OnFocusDashEnded(AbilityEndedData.bWasCancelled);
		UE_LOG(LogTemp, Warning, TEXT("ccc LOCK FOCUS"));
	}
	else if (UGameplayAbility_FocusPeriod* _endedPeriod = Cast<UGameplayAbility_FocusPeriod>(AbilityEndedData.AbilityThatEnded))
	{
		CurrentLockedOnFocus = nullptr; // Clear any FocusBase that was locked onto
		
		// Toggle HUD
		PlayerHUDWidget->ToggleFocusLockedHUDElements(false);
		
		OnFocusPeriodEnded(AbilityEndedData.bWasCancelled);
		UE_LOG(LogTemp, Warning, TEXT("ccc UN LOCK FOCUS"));
	}
}

void AFocusfireCharacter::HandleHealthChanged(const FOnAttributeChangeData& HealthAttribute)
{
	if (HealthAttribute.NewValue <= 0)
	{
		AActor* _Killer = HealthAttribute.GEModData->EffectSpec.GetEffectContext().GetInstigator();
		float _KillerDamage = HealthAttribute.GEModData->EffectSpec.GetSetByCallerMagnitude(GameplayEffect_Damage_Tag);
		OnDeath(_Killer, _KillerDamage); // Tell BP to play animations
	}
}

FVector AFocusfireCharacter::GetPlayerPivotPosAroundLockedFocus(const AFocusBase* LockedFocus, const float IdealDistance)
{
	// Vector from Player to Player's first-person POV camera
	FirstPersonPOVCameraOffset = GetActorLocation() - GetFirstPOVCamera()->GetComponentLocation();
		
	// Current rotation of Controller
	const FRotator _currRot = Controller->GetControlRotation();

	// Get the position of the Player to pivot around the "locked-on" FocusBase while looking with the first-person POV camera
	const FVector _nextPosition = (LockedFocus->GetActorLocation() + FirstPersonPOVCameraOffset) - _currRot.Vector() * IdealDistance;

	return _nextPosition;
}

void AFocusfireCharacter::SetGravityByMultiplier(const float NewGravityMultiplier)
{
	GetCharacterMovement()->GravityScale = DefaultGravityScale * NewGravityMultiplier;
}
