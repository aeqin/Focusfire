// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_FocusShoot.h"

#include "FocusfireCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"

void UGameplayAbility_FocusShoot::SpawnFocusToShoot()
{
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor.Get()))
	{
		FTransform _spawnTransform = _player->GetFocusSpawnArrow()->GetComponentTransform();
		TSubclassOf<AFocusBase> _typeOfFocusToSpawn = _player->GetCurrentFocusToShoot();
		AFocusBase* _spawnedFocus = GetWorld()->SpawnActorDeferred<AFocusBase>(_typeOfFocusToSpawn, _spawnTransform, _player->GetOwner(), _player->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (IsValid(_spawnedFocus))
		{
			_spawnedFocus->FinishSpawning(_spawnTransform);
			_spawnedFocus->ShootInDirection(_player->GetCurrentCamera()->GetForwardVector());
		}
	}
}
