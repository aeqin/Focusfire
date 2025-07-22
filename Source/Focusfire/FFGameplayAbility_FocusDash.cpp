// Fill out your copyright notice in the Description page of Project Settings.


#include "FFGameplayAbility_FocusDash.h"

#include "FocusfireCharacter.h"

AFocusBase* UFFGameplayAbility_FocusDash::DashToFocusInRange()
{
	DashToOffsetX = DefaultDashToOffsetX; // Reset offset
	
	// Make sure owner actor is a FocusfireCharacter
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor))
	{
		// Get the current FocusBase under the crosshair
		AFocusBase* _dashToFocus = _player->GetCurrentDashableToFocus();
		_dashToFocus->LockInPlace(true); // Lock it in place (cancel its velocity) + extend its lifetime

		// Set the offset as decreed by the FocusfireCharacter
		DashToOffsetX = _player->GetRangeOfFocusAbilityUseable() - 1.0f;

		UE_LOG(LogTemp, Display, TEXT("DebugText DashToFocusInRange"));
		return _player->GetCurrentDashableToFocus();
	}

	return nullptr;
}

bool UFFGameplayAbility_FocusDash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		if (_player->GetCurrentDashableToFocus()) 
			return true;

		// Is a nullptr, meaning there is NO valid FocusBase to dash to
		return false;
	}
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}
