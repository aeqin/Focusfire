// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_FocusMarker.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UUserWidget_FocusMarker : public UUserWidget
{
	GENERATED_BODY()

	/**
	 * @returns: The distance (in meters) from the Player to this marker location
	 **/
	UFUNCTION(BlueprintPure, Category = "FocusMarker", meta = (AllowPrivateAccess = "true"))
	int GetRoundedMDistanceToPlayer();

public:

	/** The AActor that this marker is tracking in relation to the player **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusMarker")
	AActor* ActorToTrack;
};
