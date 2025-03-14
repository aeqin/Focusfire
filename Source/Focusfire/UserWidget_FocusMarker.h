// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_FocusMarker.generated.h"

UENUM(BlueprintType)
enum class EFocusMarkerMode : uint8
{
	ON_SCREEN,
	OFF_SCREEN, // Arrow-like image that points towards FocusBase in relation to Player
};

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UUserWidget_FocusMarker : public UUserWidget
{
	GENERATED_BODY()

	/** Mode changes the image that this widget displays depending on whether FocusBase is ON/OFF Player's screen **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusMarker", meta = (AllowPrivateAccess = "true"))
	EFocusMarkerMode FocusMarkerMode = EFocusMarkerMode::ON_SCREEN;
	
	/**
	 * @returns: The distance (in meters) from the Player to this marker location
	 **/
	UFUNCTION(BlueprintPure, Category = "FocusMarker", meta = (AllowPrivateAccess = "true"))
	int GetRoundedMDistanceToPlayer();

protected:
	/**
	 * Event that fires on the BP to change the image depending on the set mode
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusMarker")
	void OnFocusMarkerModeChanged();
	
public:

	/** The AActor that this marker is tracking in relation to the player **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusMarker")
	AActor* ActorToTrack;

	/**
	 * The "offset" that allows this widget to be clearly viewed (no cutoff when position is set to edge of screen) in all directions (top, bot, left, right)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusMarker")
	FVector2D WithinViewportOffset = FVector2D(110, 84);
	
	/**
	 * Changes the mode of this FocusMarker
	 * @param NewMarkerMode: Enum, the new mode to be set 
	 */
	UFUNCTION(BlueprintCallable, Category = "FocusMarker")
	void SetFocusMarkerMode(const EFocusMarkerMode NewMarkerMode);
	
	/**
	 * Event that fires on the BP to rotate the image that represents a FocusBase offscreen from the Player (this event is ideally called every tick)
	 * @param Angle The angle to [Set Render Transform Angle] in BP
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusMarker")
	void OnTickRotateIndicatorImage(const float Angle);
};
