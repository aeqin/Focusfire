// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFGameplayAbility.h"
#include "FFGameplayAbility_FocusShoot.generated.h"

class AFocusfireCharacter;
/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UFFGameplayAbility_FocusShoot : public UFFGameplayAbility
{
	GENERATED_BODY()
	
protected:
	/** The AFocusfireCharacter that activated this ability. Set in CommitAbility() */
	UPROPERTY(BlueprintReadOnly)
	AFocusfireCharacter* PlayerFocusShootSpawner;
	
	/** Attempt to spawn a FocusBase to shoot out of a location */
	UFUNCTION(BlueprintCallable, Category = "FocusShoot")
	void SpawnFocusToShoot();

protected: /* Overloads */
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
};
