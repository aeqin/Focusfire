// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_FocusShoot.h"

#include "ActorComponent_ManagerFocus.h"
#include "FocusfireCharacter.h"
#include "FocusfireGameState.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"

void UGameplayAbility_FocusShoot::SpawnFocusToShoot()
{
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(CurrentActorInfo->AvatarActor.Get()))
	{
		FTransform _spawnTransform = _player->GetFocusSpawnArrow()->GetComponentTransform();
		FVector _spawnDirection = _player->GetCurrentCamera()->GetForwardVector();
		TSubclassOf<AFocusBase> _typeOfFocusToSpawn = _player->GetCurrentFocusToShoot();

		// Spawn using ManagerFocus, which keeps track of all FocusBase in game
		GetWorld()->GetGameState<AFocusfireGameState>()->GetManagerFocus()->ShootFocusInDirection(_spawnTransform, _spawnDirection, _typeOfFocusToSpawn, _player);
	}
}
