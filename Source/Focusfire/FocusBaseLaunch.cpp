// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusBaseLaunch.h"

#include "FocusfireCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void AFocusBaseLaunch::ActivateAbility(const AActor* Activator)
{
	if (not IsValid(Activator))
		return;
	
	if (const AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(Activator))
	{
		// Launch Player from locked position
		FVector _forwardVector = _player->GetCurrentCamera()->GetForwardVector();
		FVector _rightVector = _player->GetCurrentCamera()->GetRightVector();
		_forwardVector = _forwardVector.RotateAngleAxis(LaunchAngle, _rightVector); // Launch slightly upwards
		_player->GetCharacterMovement()->Launch(_forwardVector * LaunchSpeed);

		// Debugging
		/*FVector Start = _player->GetCurrentCamera()->GetComponentLocation();
		FVector End = Start + _forwardVector * LaunchSpeed;
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3, 0, 5);*/
	}

	// Destroy once ability is used
	Destroy();
}
