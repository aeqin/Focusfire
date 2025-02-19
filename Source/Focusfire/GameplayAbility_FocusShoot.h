// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_FocusShoot.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UGameplayAbility_FocusShoot : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	/** Attempt to spawn a FocusBase to shoot out of a location */
	UFUNCTION(BlueprintCallable, Category = "FocusShoot")
	void SpawnFocusToShoot();
};
