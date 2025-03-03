// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_FocusMeteor.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UGameplayAbility_FocusMeteor : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	/** The speed at which to slam down the Activator **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "FocusMeteor")
	float DownForce = 3000.0f;

	/** The radius of the resulting ground smash **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "FocusMeteor")
	float GroundSmashRadius = 400.0f;

	/** The half-height of the resulting ground smash **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "FocusMeteor")
	float GroundSmashOffset_Y = 300.0f;

	/** The MINIMUM damage of the resulting ground smash **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "FocusMeteor")
	float GroundSmash_DefaultDamage = 100.0f;

	/** The MAXIMUM multiplier of damage of the resulting ground smash (based on GroundSmash_DefaultDamage) **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "FocusMeteor")
	float GroundSmash_MaximumMultiplier = 4.0f;

	/** The MINIMUM distance activator must fall before doing extra damage **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "FocusMeteor")
	float GroundSmash_MinFall = 100.0f;

	/** The MAXIMUM distance activator can fall to reach max damage **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "FocusMeteor")
	float GroundSmash_MaxFall = 1000.0f;
	
	/** VFX that represents ground smash */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusMeteor")
	TObjectPtr<UNiagaraSystem> NiagaraSystem_GroundSmashVFX;
};
