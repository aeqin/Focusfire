// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusfireGameState.h"

#include "ActorComponent_ManagerFocus.h"

AFocusfireGameState::AFocusfireGameState()
{
	Manager_Focus = CreateDefaultSubobject<UActorComponent_ManagerFocus>(TEXT("Manager_Focus"));
}
