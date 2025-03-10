// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "FocusBase.generated.h"

class UWidgetComponent;
class UUserWidget_FocusMarker;
class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class FOCUSFIRE_API AFocusBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFocusBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Sphere collision */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> c_SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	FCollisionProfileName CollisionProfile = FName("Focus");

	/** Component to hold VFX that represents FocusBase in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraComponent> c_NiagaraComponent_RepresentingVFX;

	/** VFX to represent FocusBase in game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> NiagaraSystem_RepresentingVFX;

	/** VFX that represents FocusBase hitting something */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNiagaraSystem> NiagaraSystem_HitVFX;
	
	/** Projectile Motion */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* c_ProjectileMovementComponent;
	
	/** Shoot speed, the speed at which the FocusBase is shot out at */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	float ShootSpeed = 1000.0f;
	
	/** The FocusMarkerWidget ref that displays distance from FocusBase to player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FocusBase")
	UUserWidget_FocusMarker* FocusMarkerWidget;

	/** The widget component that holds the BP class of the FocusMarker widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FocusBase")
	UWidgetComponent* FocusMarkerWidgetComponent;
	
	/** Max Lifetime, how many seconds the FocusBase will last before being automatically destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	float LifeInSecondsMax = 10.0f;

	/** Current lifetime, how many seconds the FocusBase will last before being automatically destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	float LifeInSecondsCurr = 10.0f;

	FTimerHandle LifeTimerHandle;
	
	/** 
	* Every tick, decrement a timer. Once the timer has ended, destroy this FocusBase
	*/
	void TickLifetimeTimer();

	/** Whether this FocusBase can be "locked-on" by the player (FocusfireCharacter) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	bool flag_Interactable = true;

	/** The default damage that this FocusBase deals */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusBase", meta = (AllowPrivateAccess = "true"))
	float DamageDefault = 100.0f;

	/** 
	* Called in response to the OnComponentBeginOverlap event
	* @param OverlappedComponent: A pointer to the component that triggered the event (i.e., the component of the actor receiving the event).
	* @param OtherActor:  A pointer to the actor that overlapped with the component.
	* @param OtherComp: A pointer to the component of the other actor that overlapped with the component.
	* @param OtherBodyIndex: The index of the body (in case of skeletal meshes or complex collision) of the other component that overlapped.
	* @param bFromSweep: Indicates whether the overlap was the result of a sweep (movement with collision detection) or not.
	* @param SweepResult: If bFromSweep is true, this contains detailed information about the sweep hit. Otherwise, it will contain default values.
	*/
	UFUNCTION()
	virtual void OnFocusBaseActorEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnFocusBaseActorExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/** GameplayAbilitySystem **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> c_AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return c_AbilitySystemComponent; }

	/** 
	* Shoot the FocusBase forward at speed
	* @param Direction: The direction at which to shoot the FocusBase
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusBase")
	void ShootInDirection(const FVector Direction);

	/** 
	* Lock the FocusBase in place (cancel velocity)
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusBase")
	void LockInPlace(bool bResetLifetime = false);

	/** 
	* Activate the special ability of this FocusBase (overridden in children)
	* @param Activator: The AActor that activated this ability
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusBase")
	virtual void ActivateAbility(const AActor* Activator);

public:
	/** Getters */
	
	/** 
	* @returns Whether the player can interact with this FocusBase
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusBase")
	FORCEINLINE bool GetCanBeInteractedWith() const { return flag_Interactable; }
};
