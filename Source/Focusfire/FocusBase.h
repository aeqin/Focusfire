// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "FocusBase.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> c_SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FCollisionProfileName CollisionProfile = FName("Focus");

	/** Projectile Motion */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* c_ProjectileMovementComponent;
	
	/** Shoot speed, the speed at which the FocusBase is shot out at */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ShootSpeed = 100.0f;

	/** Max Lifetime, how many seconds the FocusBase will last before being automatically destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LifeInSecondsMax = 10.0f;

	/** Current lifetime, how many seconds the FocusBase will last before being automatically destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LifeInSecondsCurr = 10.0f;

	FTimerHandle LifeTimerHandle;
	
	/** 
	* Every tick, decrement a timer. Once the timer has ended, destroy this FocusBase
	*/
	void TickLifetimeTimer();

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
};
