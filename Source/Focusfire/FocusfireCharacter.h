// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "FocusBase.h"
#include "GameplayTagContainer.h"
#include "FocusfireCharacter.generated.h"

struct FGameplayAbilitySpecHandle;
class UGameplayAbility;
struct FAbilityEndedData;
class AFocusBase;
class USpringArmComponent;
class UCameraComponent;
class UTimelineComponent;
class UInputMappingContext;
class UInputAction;
class UAbilitySystemComponent;
class UAttributeSetHealth;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusInRangeChanged, AFocusBase*, FocusInRange);

UCLASS(config=Game)
class AFocusfireCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* c_CameraBoom;

	/** Third Person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* c_ThirdPOVCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* c_ThirdPOVArrow;

	/** First Person camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* c_FirstPOVCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* c_FirstPOVArrow;

	UPROPERTY(BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CurrentCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* c_FocusShootArrow;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** UseFocusAbility Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* UseFocusAbilityAction;

public:
	AFocusfireCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for using focus abilities input */
	void UseFocusAbility(const FInputActionValue& Value);

	/** Used to reset Player's default gravity after ending a "locked-on" FocusBase state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusfireCharacter")
	float DefaultGravityScale = 1.75f;

	/** The offset from the first-person POV camera to the Player's position. */
	UPROPERTY(BlueprintReadOnly, Category = "FocusfireCharacter")
	FVector FirstPersonPOVCameraOffset;

protected: /* Switching camera POV */
	/** 
	* Based on the POV of the CurrentCamera, lerp the in-between position
	* @param Alpha: The current lerp progress. Ideally from the SwitchCameraTimeline in BP_FocusfireCharacter
	* @return The in-progress lerped position of the CurrentCamera each frame
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusfireCharacter")
	FVector LerpCurrentCameraLocation(const float Alpha);

	/** Whether Player is currently switching between POV cameras */
	UPROPERTY(BlueprintReadOnly, Category = "FocusfireCharacter")
	bool flag_isCurrentlySwitchingCamera = false;
	
	/** 
	* Called before timeline transition, toggle bUseControllerRotationYaw depending on POV of CurrentCamera
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusfireCharacter")
	void SwitchCameraBegin();
	
	/** 
	* Called after timeline transition, switch and set the Player's CurrentCamera between the first and third person view
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusfireCharacter")
	void SwitchCameraEnd();

protected: /* FocusBase under crosshair */
	/** The range at which a FocusBase's ability is use-able */
	UPROPERTY(BlueprintReadWrite, Category = "FocusfireCharacter")
	float RangeOfFocusAbilityUseable = 100.0f;
	
	/** The range at which a FocusBase is dash-able to */
	UPROPERTY(BlueprintReadWrite, Category = "FocusfireCharacter")
	float RangeOfDashableToFocusRaycast = 2000.0f;
	
	/** The current FocusBase in range that can be dashed to with "GameplayAbility.Focus.Dash", under crosshair */
	UPROPERTY(BlueprintReadOnly, Category = "FocusfireCharacter")
	AFocusBase* CurrentDashableToFocus;
	
	/** 
	* Called every Tick(), during the "GameplayAbility.Focus.Period". Sets CurrentDashableToFocus, if there is any FocusBase
	* in range
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusfireCharacter")
	void OnTickRaycastForDashableToFocus();

	/** 
	* Event that fires if CurrentDashableToFocus changes
	*/
	UPROPERTY(BlueprintAssignable, Category = "FocusfireCharacter")
	FOnFocusInRangeChanged OnDashableToFocusChanged;

	/** The current FocusBase within range of using its ability */
	UPROPERTY(BlueprintReadOnly, Category = "FocusfireCharacter")
	AFocusBase* CurrentLockedOnFocus;

	/**
	 * The initial distance from a locked on FocusBase. Used to keep Player the same distance away from a "locked-on"
	 * FocusBase while pivoting around it
	 */
	UPROPERTY(BlueprintReadOnly, Category = "FocusfireCharacter")
	float LockedOnFocusDistance;

	/** 
	* Called in Look(), if Player has a "locked-on" FocusBase, then pivot Player around it
	*/
	void PivotAroundLockedFocus();
	
protected: /* Shooting FocusBase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusfireCharacter")
	TSubclassOf<AFocusBase> TypeOfFocusToShoot = AFocusBase::StaticClass();
	
protected: /* GameplayAbilitySystem */
	/** AbilitySystemComponent **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> c_AbilitySystemComponent;

	/** The GameplayTag that denotes if the Player is currently in the "GameplayAbility.Focus.Period" state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusfireCharacter")
	FGameplayTag DuringFocusPeriodTag;

	/** 
	* Event that is fired when BP should try to activate "GameplayAbility.Focus.Dash"
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusfireCharacter")
	void OnInputFocusDash();

	/** 
	* Event that is fired when BP should try to activate "GameplayAbility.Focus.Shoot"
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusfireCharacter")
	void OnInputFocusShoot();

	/** 
	* Event that is fired when BP should try to activate the custom "GameplayAbility" of the locked on FocusBase
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusfireCharacter")
	void OnInputFocusBaseActivateAbility(AFocusBase* LockedOnFocus);
	
	/** 
	* Function that is called when OnAbilityEnded event is fired from AbilitySystemComponent
	* @param ActivatedAbility Contains info about the GameplayAbility that just started
	*/
	void OnGameplayAbilityStarted(UGameplayAbility* ActivatedAbility);
	
	/** 
	* Function that is called when OnAbilityEnded event is fired from AbilitySystemComponent
	* @param AbilityEndedData Contains info about the GameplayAbility that just ended
	*/
	void OnGameplayAbilityEnded(const FAbilityEndedData& AbilityEndedData);

	/** 
	* Event that is fired when the FocusDash GameplayAbility has just ended
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusfireCharacter")
	void OnFocusDashEnded(const bool bWasCancelled);

	/** 
	* Event that is fired when the FocusPeriod GameplayAbility has just ended
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusfireCharacter")
	void OnFocusPeriodEnded(const bool bWasCancelled);
	
public:
	/** Cameras **/
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return c_CameraBoom; }
	/** Returns Third Person Camera **/
	FORCEINLINE UCameraComponent* GetThirdPOVCamera() const { return c_ThirdPOVCamera; }
	/** Returns First Person Camera **/
	FORCEINLINE UCameraComponent* GetFirstPOVCamera() const { return c_FirstPOVCamera; }
	/** Returns The current POV Camera **/
	FORCEINLINE UCameraComponent* GetCurrentCamera() const { return CurrentCamera; }
	
	UFUNCTION(BlueprintPure, Category = "FocusfireCharacter")
	FORCEINLINE bool GetCameraIsCurrentlyFirstPersonPOV() const { return CurrentCamera == c_FirstPOVCamera; }

	/** Returns The Ability System Component **/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return c_AbilitySystemComponent; }

	/** Returns The current FocusBase in range to be dashed to **/
	FORCEINLINE AFocusBase* GetCurrentDashableToFocus() const { return CurrentDashableToFocus; }

	/** Returns The current FocusBase in range to have its ability used **/
	FORCEINLINE AFocusBase* GetCurrentLockedOnFocus() const { return CurrentLockedOnFocus; }

	/** Returns The arrow component of where to spawn FocusBase **/
	FORCEINLINE UArrowComponent* GetFocusSpawnArrow() const { return c_FocusShootArrow; }
	
	/** Returns The current focus to spawn (to shoot) **/
	FORCEINLINE TSubclassOf<AFocusBase> GetCurrentFocusToShoot() const { return TypeOfFocusToShoot; }

	/** Returns the range at which a FocusBase's ability is use-able **/
	FORCEINLINE float GetRangeOfFocusAbilityUseable() const { return RangeOfFocusAbilityUseable; }
	
	/** 
	* Sets the Character's gravity scale (based on its default gravity) by a multiplier
	* @param NewGravityMultiplier The new gravity multiplier
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusfireCharacter")
	void SetGravityByMultiplier(const float NewGravityMultiplier);
	
	/* GameplayAbilitySystem Attributes */
	UPROPERTY()
	TObjectPtr<UAttributeSetHealth> as_HealthAttributeSet;

	/** 
	* When Player's Health is changed, handle it
	* @param Magnitude: How much is the Health changed by
	* @param NewHealth: The new Health
	* @return
	*/
	UFUNCTION()
	void HandleHealthChanged(float Magnitude, float NewHealth);
};

