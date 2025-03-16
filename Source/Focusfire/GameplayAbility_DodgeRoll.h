// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_DodgeRoll.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UGameplayAbility_DodgeRoll : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	/** The initial speed of the dodge roll */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeRoll")
	float DodgeForce = 450.0f;
	
	/** Controls the "acceleration" of the dodge roll */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeRoll")
	UCurveFloat* DodgeCurve;
	
	/** Animation of the dodge roll */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DodgeRoll")
	UAnimMontage* DodgeAnimMontage;
};
