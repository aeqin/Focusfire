// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_FocusPeriod.h"

#include "FocusfireCharacter.h"
#include "Chaos/SoftsExternalForces.h"
#include "GameFramework/CharacterMovementComponent.h"

UGameplayAbility_FocusPeriod::UGameplayAbility_FocusPeriod()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_FocusPeriod::TickPeriodTimer()
{
	TimeLeftOfFocusPeriodTime -= 1; // Timer ticks down by 1 every second
	if (TimeLeftOfFocusPeriodTime < 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle); // Clear timer
		UGameplayAbility_FocusPeriod::EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false); // End "focus period" ability
	}
}

void UGameplayAbility_FocusPeriod::FocusPeriodStart()
{
	// Slow down Player while in focus period ability
	CurrentActorInfo->AvatarActor->CustomTimeDilation = SlowTimeDilation;

	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor))
	{
		if (_player->GetFocusSpawnArrow())
		{
			// Disable Player gravity & set velocity to zero
			_player->SetGravityByMultiplier(0.0);
			_player->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
			UE_LOG(LogTemp, Warning, TEXT("ccc FREEZE PLAYER DURING LOCKED FOCUS"));
		}
	}
	
	// TODO: Also slow down other FocusBase/Player's in radius?
}

void UGameplayAbility_FocusPeriod::FocusPeriodEnd()
{
	// Return Player back to normal time
	CurrentActorInfo->AvatarActor->CustomTimeDilation = 1;

	// Set Player gravity back to default
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor))
	{
		_player->SetGravityByMultiplier(1.0);
	}

	// Clear running timer
	GetWorld()->GetTimerManager().ClearTimer(PeriodTimerHandle);

	UE_LOG(LogTemp, Warning, TEXT("ccc UGameplayAbility_FocusPeriod::FocusPeriodEnd"));
}

bool UGameplayAbility_FocusPeriod::CommitAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 FGameplayTagContainer* OptionalRelevantTags)
{
	// Create a timer that ticks down, when over, then end the "focus period" ability
	TimeLeftOfFocusPeriodTime = MaxLengthOfFocusPeriodTime;
	GetWorld()->GetTimerManager().SetTimer(PeriodTimerHandle, this, &UGameplayAbility_FocusPeriod::TickPeriodTimer,
	                                       1.0f, true);

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void UGameplayAbility_FocusPeriod::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              bool bReplicateEndAbility, bool bWasCancelled)
{
	// Make sure time dilation is set back to normal
	UGameplayAbility_FocusPeriod::FocusPeriodEnd();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
