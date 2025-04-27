// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_FocusPeriod.generated.h"

class AFocusfireCharacter;
class AFocusPeriodSlowZone;
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
	/** The AFocusfireCharacter that activated this ability */
	UPROPERTY(BlueprintReadOnly)
	AFocusfireCharacter* PlayerFocusPeriodSpawner;

	/** The type of AFocusPeriodSlowZone to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AFocusPeriodSlowZone> FocusPeriodSlowZoneClass;
	
	/** How slow should the time be during "focus" state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowTimeDilation = 0.2f;

	/** How slow should the time be during "focus" state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowTimeRadius = 2000.0f;
	
	/** The sphere area of effect that slows down AActors */
	UPROPERTY(BlueprintReadOnly)
	AFocusPeriodSlowZone* SpawnedFocusedPeriodSlowZone;
	
	/** How long the "focus" state will last */
	UPROPERTY(EditAnywhere)
	int32 MaxLengthOfFocusPeriodTime;
	
	int32 TimeLeftOfFocusPeriodTime;

	FTimerHandle PeriodTimerHandle;

	/** 
	* Every second, decrement a timer. Once the timer has ended, end the "focus period" ability
	*/
	void TickPeriodTimer();

	/** 
	* Checks that the activating AActor is a valid Player and stores it in PlayerFocusPeriodSpawner
	* @returns: Whether the activator for this ability is valid, if false, then BP should end the ability
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusPeriod")
	bool OnFocusPeriodStartCheck();

	/**
	 * After the FocusPeriodSlowZone is spawned on server, initialize it
	 * @param SpawnedFocusedSlowZone: The spawned AFocusPeriodSlowZone
	 * @return A AFocusPeriodSlowZone that is initialized (attached to spawner)
	 */
	UFUNCTION(BlueprintCallable, Category = "FocusPeriod")
	void OnFocusPeriodSlowZoneSpawned(AFocusPeriodSlowZone* SpawnedFocusedSlowZone);
	
	/** 
	* Un-freeze the player locked in place, and re-add player to be once again slowed by SpawnedFocusedPeriodSlowZone
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusPeriod")
	void FocusPeriodCancelLock();
	
	/** 
	* Return the Player to normal speed
	*/
	UFUNCTION(BlueprintCallable, Category = "FocusPeriod")
	void FocusPeriodEnd();
	
protected: /* Overloads */
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
