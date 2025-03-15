// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility_Ping.h"

#include "ActorComponent_ManagerFocus.h"
#include "FocusfireCharacter.h"
#include "FocusfireGameState.h"
#include "PingSphere.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameplayAbility_Ping::SpawnProspectivePing()
{
	AActor* _activator = CurrentActorInfo->AvatarActor.Get();
	if (not _activator) return;

	// Get start and end locations of ping
	FVector _TraceStart;
	FVector _TraceEnd;
	if (AFocusfireCharacter* _player = Cast<AFocusfireCharacter>(_activator))
	{
		// Need to trace from camera in case of FocusfireCharacter
		FollowCamera = _player->GetCurrentCamera();
		_TraceStart = FollowCamera->GetComponentLocation();
		_TraceEnd = _TraceStart + FollowCamera->GetForwardVector() * MaximumPingDistance;
	}
	else
	{
		_TraceStart = _activator->GetActorLocation();
		_TraceEnd = _TraceStart + _activator->GetActorForwardVector() * MaximumPingDistance;
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
		PingCurrentRange = MaximumPingDistance;
		_spawnPingTransform.SetLocation(_TraceEnd);
	}

	// Spawn prospective PingSphere
	if (SpawnedPingSphereClass)
	{
		// Spawn using ManagerFocus, which keeps track of all PingSphere in game
		SpawnedPingSphere = GetWorld()->GetGameState<AFocusfireGameState>()->GetManagerFocus()->SpawnPing(_spawnPingTransform, SpawnedPingSphereClass, _activator);
	}

	// If activator has a camera, aim the PingSphere relative to the camera
	if (FollowCamera)
	{
		SpawnedPingSphere->AttachToComponent(FollowCamera, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void UGameplayAbility_Ping::OnInputMovePingFurther()
{
	if (not IsValid(FollowCamera)) return;

	// Don't move further than max distance
	FVector _CamLocation = FollowCamera->GetComponentLocation();
	FVector _CurrPingLocation = SpawnedPingSphere->GetActorLocation();
	FVector _PingFurtherDirection = (_CurrPingLocation - _CamLocation).GetSafeNormal();
	if ((_CurrPingLocation - _CamLocation).Length() > MaximumPingDistance - OnInputPingMoveDist)
	{
		SpawnedPingSphere->SetActorLocation(_CamLocation + _PingFurtherDirection * MaximumPingDistance); // Set at max distance
	}
	else
	{
		SpawnedPingSphere->SetActorLocation(_CurrPingLocation + _PingFurtherDirection * OnInputPingMoveDist); // Move further
	}
}

void UGameplayAbility_Ping::OnInputMovePingCloser()
{
	if (not IsValid(FollowCamera)) return;

	// Don't move closer than min distance
	FVector _CamLocation = FollowCamera->GetComponentLocation();
	FVector _CurrPingLocation = SpawnedPingSphere->GetActorLocation();
	FVector _PingFurtherDirection = (_CurrPingLocation - _CamLocation).GetSafeNormal();
	if ((_CurrPingLocation - _CamLocation).Length() < MinimumPingDistance + OnInputPingMoveDist)
	{
		SpawnedPingSphere->SetActorLocation(_CamLocation + _PingFurtherDirection * MinimumPingDistance); // Set at min distance
	}
	else
	{
		SpawnedPingSphere->SetActorLocation(_CurrPingLocation - _PingFurtherDirection * OnInputPingMoveDist); // Move closer
	}
}

void UGameplayAbility_Ping::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// If there remains a valid SpawnedPingSphere, then detach from FollowCamera (Confirming the prospective PingSphere to its current location)
	if (not bWasCancelled && IsValid(SpawnedPingSphere))
	{
		SpawnedPingSphere->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}
