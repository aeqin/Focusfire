// Fill out your copyright notice in the Description page of Project Settings.


#include "FFGameplayAbility_FocusShoot.h"

#include "ActorComponent_ManagerFocus.h"
#include "FocusfireCharacter.h"
#include "FocusfireGameState.h"
#include "PingSphere.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"

bool UFFGameplayAbility_FocusShoot::CommitAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags)
{
	// Store Valid Player
	PlayerFocusShootSpawner = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor.Get());
	
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}
