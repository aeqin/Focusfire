// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_FocusDash.h"

#include "FocusfireCharacter.h"

AFocusBase* UGameplayAbility_FocusDash::DashToFocusInRange()
{
	// Make sure owner actor is a FocusfireCharacter
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor))
	{
		// Get the current "focus" under the crosshair
		UE_LOG(LogTemp, Display, TEXT("ccc DashToFocusInRange"));
		return _player->GetCurrentFocusInRange();
	}

	return nullptr;
}

bool UGameplayAbility_FocusDash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// No owner actor, return false
	if (ActorInfo == nullptr)
		return false;
	
	// Make sure owner actor is a FocusfireCharacter
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(ActorInfo->AvatarActor))
	{
		// Not a nullptr, so there is a valid FocusBase in range
		if (_player->GetCurrentFocusInRange()) 
			return true;

		// Is a nullptr, meaning there is NO valid FocusBase to dash to
		return false;
	}
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}
