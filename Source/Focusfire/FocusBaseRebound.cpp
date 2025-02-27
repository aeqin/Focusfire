// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusBaseRebound.h"

#include "FocusfireCharacter.h"
#include "Camera/CameraComponent.h"

void AFocusBaseRebound::ActivateAbility(const AActor* Activator)
{
	if (not IsValid(Activator))
		return;

	
	if (const AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(Activator))
	{
		FVector _forwardVector = _player->GetCurrentCamera()->GetForwardVector();

		// Reset lifetime timer
		LifeInSecondsCurr = LifeInSecondsMax;
		
		// Set FocusBaseRebound to shoot faster, and shrink its scale as well
		SetActorScale3D(GetActorScale3D() / ReboundRatio);
		ShootSpeed *= ReboundRatio;

		// Shoot FocusBaseRebound in direction Player is aiming
		AFocusBase::ShootInDirection(_forwardVector);

		// Once MaxBounces is reached, become NON-interactable
		NumBounces += 1;
		if (NumBounces >= MaxBounces)
		{
			flag_Interactable = false;
		}
	}
}
