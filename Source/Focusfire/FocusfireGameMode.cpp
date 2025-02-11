// Copyright Epic Games, Inc. All Rights Reserved.

#include "FocusfireGameMode.h"
#include "FocusfireCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFocusfireGameMode::AFocusfireGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_FocusfireCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
