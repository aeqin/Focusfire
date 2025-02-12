// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "FocusfireCharacter.generated.h"

/** Forward Declarations */
class USpringArmComponent;
class UCameraComponent;
class UTimelineComponent;
class UInputMappingContext;
class UInputAction;
class UAbilitySystemComponent;
class UAttributeSetHealth;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

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

public:
	AFocusfireCharacter();
	virtual void BeginPlay() override;

protected:
	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** 
	* Based on the POV of the CurrentCamera, lerp the in-between position
	* @param float Alpha: The current lerp progress. Ideally from the SwitchCameraTimeline in BP_FocusfireCharacter
	* @return FVector -- The in-progress lerped position of the CurrentCamera each frame
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusfireCharacter")
	FVector LerpCurrentCameraLocation(const float Alpha);

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
	
public:
	/** Cameras **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return c_CameraBoom; } /** Returns CameraBoom subobject **/
	FORCEINLINE UCameraComponent* GetThirdPOVCamera() const { return c_ThirdPOVCamera; } /** Returns Third Person Camera **/
	FORCEINLINE UCameraComponent* GetFirstPOVCamera() const { return c_FirstPOVCamera; } /** Returns First Person Camera **/
	FORCEINLINE UCameraComponent* GetCurrentCamera() const { return CurrentCamera; }
	

	/** GameplayAbilitySystem **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> c_AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return c_AbilitySystemComponent; }

	/* GameplayAbilitySystem Attributes*/
	UPROPERTY()
	TObjectPtr<UAttributeSetHealth> as_HealthAttributeSet;

	/** 
	* When Player's Health is changed, handle it
	* @param float Magnitude: How much is the Health changed by
	* @param float NewHealth: The new Health
	* @return
	*/
	UFUNCTION()
	void HandleHealthChanged(float Magnitude, float NewHealth);
};

