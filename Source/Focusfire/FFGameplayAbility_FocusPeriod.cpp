// Fill out your copyright notice in the Description page of Project Settings.


#include "FFGameplayAbility_FocusPeriod.h"

#include "FocusfireCharacter.h"
#include "FocusPeriodSlowZone.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tasks/GameplayTask_SpawnActor.h"

UFFGameplayAbility_FocusPeriod::UFFGameplayAbility_FocusPeriod()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFFGameplayAbility_FocusPeriod::TickPeriodTimer()
{
	TimeLeftOfFocusPeriodTime -= 1; // Timer ticks down by 1 every second
	if (TimeLeftOfFocusPeriodTime < 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle); // Clear timer
		UFFGameplayAbility_FocusPeriod::EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); // End "focus period" ability
	}
}

bool UFFGameplayAbility_FocusPeriod::OnFocusPeriodStartCheck()
{
	// Check Valid Player
	PlayerFocusPeriodSpawner = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor.Get());
	if (IsValid(PlayerFocusPeriodSpawner))
	{
		return true;
	}
	else
	{
		return false;	
	}
}

void UFFGameplayAbility_FocusPeriod::OnFocusPeriodSlowZoneSpawned(AFocusPeriodSlowZone* SpawnedFocusedSlowZone)
{
	// Once FocusPeriodSpawner has been spawned on server, attach it to Player
	SpawnedFocusedPeriodSlowZone = SpawnedFocusedSlowZone;
	if (IsValid(SpawnedFocusedPeriodSlowZone))
	{
		SpawnedFocusedPeriodSlowZone->AttachToActor(PlayerFocusPeriodSpawner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SpawnedFocusedPeriodSlowZone->RadiusOfSlowEffect = SlowTimeRadius;
		SpawnedFocusedPeriodSlowZone->StrengthOfTimeSlowdown = SlowTimeDilation;
		SpawnedFocusedPeriodSlowZone->SetAndSlowSpawnerActor(PlayerFocusPeriodSpawner);

		// If Player is locked on to a Focus, then disable Player's gravity
		if (PlayerFocusPeriodSpawner->GetCurrentLockedOnFocus())
		{
			// Disable Player gravity & set velocity to zero
			PlayerFocusPeriodSpawner->SetGravityByMultiplier(0.0);
			PlayerFocusPeriodSpawner->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
			UE_LOG(LogTemp, Warning, TEXT("DebugText FREEZE PLAYER DURING LOCKED FOCUS"));

			// As Player is "locked" in place to a FocusBase, reset Player's time dilation, to allow Player to pivot around
			// FocusBase at normal speed
			SpawnedFocusedPeriodSlowZone->SetActorSlowdownException(PlayerFocusPeriodSpawner, true);
		}
	}
}

void UFFGameplayAbility_FocusPeriod::FocusPeriodCancelLock()
{
	if (IsValid(PlayerFocusPeriodSpawner))
	{
		if (PlayerFocusPeriodSpawner->GetCurrentLockedOnFocus() == nullptr) // Player itself will unset its CurrentLockedOnFocus
		{
			// Re-enable Player gravity
			PlayerFocusPeriodSpawner->SetGravityByMultiplier(1.0);

			// As Player is no longer "locked" in place to a FocusBase, re-slow down Player's time dilation
			SpawnedFocusedPeriodSlowZone->SetActorSlowdownException(PlayerFocusPeriodSpawner, false);
		}
	}
}

void UFFGameplayAbility_FocusPeriod::FocusPeriodEnd()
{
	// Set Player gravity back to default
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor))
	{
		_player->SetGravityByMultiplier(1.0);
	}

	// Destroy the area of effect that slows down Player and FocusBase
	if (IsValid(SpawnedFocusedPeriodSlowZone))
	{
		SpawnedFocusedPeriodSlowZone->Destroy();
	}

	// Clear running timer
	GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle);

	UE_LOG(LogTemp, Warning, TEXT("DebugText UFFGameplayAbility_FocusPeriod::FocusPeriodEnd"));
}

bool UFFGameplayAbility_FocusPeriod::CommitAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 FGameplayTagContainer* OptionalRelevantTags)
{
	// Create a timer that ticks down, when over, then end the "focus period" ability
	TimeLeftOfFocusPeriodTime = MaxLengthOfFocusPeriodTime;
	GetWorld()->GetTimerManager().SetTimer(PeriodTimerHandle, this, &UFFGameplayAbility_FocusPeriod::TickPeriodTimer,
	                                       1.0f, true);

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UFFGameplayAbility_FocusPeriod::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              bool bReplicateEndAbility, bool bWasCancelled)
{
	// Make sure time dilation is set back to normal
	UFFGameplayAbility_FocusPeriod::FocusPeriodEnd();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
