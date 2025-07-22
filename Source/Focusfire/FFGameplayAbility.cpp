// Fill out your copyright notice in the Description page of Project Settings.


#include "FFGameplayAbility.h"

#include "FocusfireGameState.h"

UActorComponent_ManagerFocus* UFFGameplayAbility::GetFocusManager()
{
	if (ManagerFocus == nullptr)
	{
		ManagerFocus = GetWorld()->GetGameState<AFocusfireGameState>()->GetManagerFocus();
	}
	return ManagerFocus;
}
