// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusBaseMeteor.h"

#include "AbilitySystemComponent.h"
#include "FocusfireCharacter.h"

void AFocusBaseMeteor::ActivateAbility(const AActor* Activator)
{
	if (not IsValid(Activator))
		return;

	// Give Player a 1-time use GameplayAbility, that slams down Player and releases a shockwave of damage
	if (const AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(Activator))
	{
		FGameplayAbilitySpec MeteorSpec(GameplayAbility_FocusMeteor, 1, 0, this);
		_player->GetAbilitySystemComponent()->GiveAbilityAndActivateOnce(MeteorSpec);
	}
}
