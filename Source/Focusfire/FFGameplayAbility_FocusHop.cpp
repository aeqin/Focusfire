// Fill out your copyright notice in the Description page of Project Settings.


#include "FFGameplayAbility_FocusHop.h"

#include "FocusfireCharacter.h"

bool UFFGameplayAbility_FocusHop::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
                                                   const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool flag_Activatable = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
	// No owner actor, return false
	if (ActorInfo == nullptr)
		return false;
	
	// Make sure owner actor is a FocusfireCharacter
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(ActorInfo->AvatarActor))
	{
		if (not flag_Activatable)
		{
			return flag_Activatable;
		}
		return true; // TODO make focus hop dependent on some resouce
	}
	
	return flag_Activatable;
}
