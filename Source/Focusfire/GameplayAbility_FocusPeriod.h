// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_FocusPeriod.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UGameplayAbility_FocusPeriod : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbility_FocusPeriod();

protected:
	// How long the "focus" state will last
	UPROPERTY(EditAnywhere)
	int32 MaxLengthOfFocusPeriodTime;
	
	int32 TimeLeftOfFocusPeriodTime;

	FTimerHandle PeriodTimerHandle;

	// How slow should the time be during "focus" state
	UPROPERTY(EditAnywhere)
	float SlowTimeDilation = 0.2f;

	/** 
	* Every second, decrement a timer. Once the timer has ended, end the "focus period" ability
	*/
	void TickPeriodTimer();

	/** 
	* Slow down the Player, and any related Actors in a sphere radius
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusPeriod")
	void FocusPeriodStart();

	/** 
	* Return the Player to normal speed
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusPeriod")
	void FocusPeriodEnd();
	
protected: /* Overloads */
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
