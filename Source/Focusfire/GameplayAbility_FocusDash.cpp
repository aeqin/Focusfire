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
