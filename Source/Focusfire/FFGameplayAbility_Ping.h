// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFGameplayAbility.h"
#include "FFGameplayAbility_Ping.generated.h"

class UCameraComponent;
class APingSphere;
/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UFFGameplayAbility_Ping : public UFFGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ping", meta = (AllowPrivateAccess = "true"))
	float CurrentRaycastHitDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ping", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APingSphere> SpawnedPingSphereClass;
	
	UPROPERTY(BlueprintReadWrite, Category="Ping", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APingSphere> SpawnedPingSphere;
	
	/** 
	* Calculate initial range of ping and spawn a prospective PingSphere
	*/
	UFUNCTION(BlueprintCallable, Category = "Ping")
	void SpawnProspectivePing();

	/** 
	* Every tick, spawn a raycast, if ping is beyond location where raycast hits an obstruction, then move the ping to the new hit location
	*/
	UFUNCTION(BlueprintCallable, Category = "Ping")
	void OnTickRaycastForPingDistance();

	/** The camera component to position the PingSphere relative to */
	UPROPERTY(BlueprintReadWrite, Category="Ping", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	/** The MINimum distance the prospective PingSphere is allowed to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ping", meta = (AllowPrivateAccess = "true"))
	float MinimumPingDistance = 100.0f;

	/** The MAXimum distance the prospective PingSphere is allowed to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ping", meta = (AllowPrivateAccess = "true"))
	float MaximumPingDistance = 2000.0f;
	
	/** How much to move the spawned PingSphere on input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ping", meta = (AllowPrivateAccess = "true"))
	float OnInputPingMoveDist = 40.0f;
	
	/** 
	* Move the spawned prospective PingSphere further away from FollowCamera
	*/
	UFUNCTION(BlueprintCallable, Category = "Ping")
	void OnInputMovePingFurther();

	/** 
	* Moved the spawned prospective PingSphere closer to FollowCamera
	*/
	UFUNCTION(BlueprintCallable, Category = "Ping")
	void OnInputMovePingCloser();

protected: /* Overloads */
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
