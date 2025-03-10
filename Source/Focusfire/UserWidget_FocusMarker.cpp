// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_FocusMarker.h"

int UUserWidget_FocusMarker::GetRoundedMDistanceToPlayer()
{
	if (IsValid(ActorToTrack))
	{
		float _DistanceInCM = (ActorToTrack->GetActorLocation() - GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation()).Length(); // Distance in centimeters
		float _DistanceInM = _DistanceInCM / 100.0f;
		return FMath::RoundToInt(_DistanceInM);
	}
	else
	{
		return -1;
	}
}
