// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_Ping.h"

#include "FocusfireCharacter.h"
#include "PingSphere.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameplayAbility_Ping::PingStart()
{
	AActor* _activator = CurrentActorInfo->AvatarActor.Get();
	if (not _activator) return;

	// Get start and end locations of ping
	FVector _TraceStart;
	FVector _TraceEnd;
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(_activator))
	{
		// Need to trace from camera in case of FocusfireCharacter
		_TraceStart = _player->GetCurrentCamera()->GetComponentLocation();
		_TraceEnd = _TraceStart + _player->GetCurrentCamera()->GetForwardVector() * PingMaxRange;
	}
	else
	{
		_TraceStart = _activator->GetActorLocation();
		_TraceEnd = _TraceStart + _activator->GetActorForwardVector() * PingMaxRange;
	}
	
	// Raycast
	const TArray<AActor*> _ActorsToIgnore = {_activator};
	const FColor _ColorBeforeHit = FColor::Green;
	const FColor _ColorAfterHit = FColor::Red;
	FHitResult _HitResult;
	FTransform _spawnPingTransform;
	if (UKismetSystemLibrary::LineTraceSingle(
		GetWorld(),
		_TraceStart,
		_TraceEnd,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		_ActorsToIgnore,
		EDrawDebugTrace::ForDuration, // Set to EDrawDebugTrace::None if not debugging, otherwise EDrawDebugTrace::ForDuration
		_HitResult,
		true,
		_ColorBeforeHit,
		_ColorAfterHit,
		3.0
	))
	{
		// Set current distance of Ping as hit location
		PingCurrentRange = _HitResult.Distance;
		_spawnPingTransform.SetLocation(_HitResult.Location);
	}
	else
	{
		// Set current distance of Ping as max distance
		PingCurrentRange = PingMaxRange;
		_spawnPingTransform.SetLocation(_TraceEnd);
	}

	// Spawn prospective PingSphere
	if (SpawnedPingSphereClass)
	{
		SpawnedPingSphere = GetWorld()->SpawnActorDeferred<APingSphere>(SpawnedPingSphereClass, _spawnPingTransform, _activator->GetOwner(), _activator->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (IsValid(SpawnedPingSphere))
		{
			SpawnedPingSphere->FinishSpawning(_spawnPingTransform);
		}
	}
}
