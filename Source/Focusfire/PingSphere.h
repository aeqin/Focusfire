// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingSphere.generated.h"

class USphereComponent;

UCLASS()
class FOCUSFIRE_API APingSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APingSphere();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Sphere collision */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PingSphere", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> c_SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PingSphere", meta = (AllowPrivateAccess = "true"))
	FCollisionProfileName CollisionProfile = FName("Focus");

	/** What the materials of the ping should be attached to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PingSphere", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> c_StaticMeshComponent;

	/** What the ping should look like when aiming */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PingSphere", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> mat_ProspectivePing;

	/** What the ping should look once active */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PingSphere", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInterface> mat_ActivePing;
	
	/** Max Lifetime, how many seconds the FocusBase will last before being automatically destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PingSphere", meta = (AllowPrivateAccess = "true"))
	float LifeInSecondsMax = 10.0f;

	/** Current lifetime, how many seconds the FocusBase will last before being automatically destroyed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PingSphere", meta = (AllowPrivateAccess = "true"))
	float LifeInSecondsCurr = 10.0f;

	FTimerHandle LifeTimerHandle;
	
	/** 
	* Every tick, decrement a timer. Once the timer has ended, destroy this FocusBase
	*/
	void TickLifetimeTimer();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Set the PingSphere as "active", which allows it to be raycasted and gives it a lifetime
	 */
	UFUNCTION(BlueprintCallable, Category = "PingSphere")
	void SetPingActive();
};
