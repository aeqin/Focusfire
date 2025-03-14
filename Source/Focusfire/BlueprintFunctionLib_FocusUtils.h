// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintFunctionLib_FocusUtils.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UBlueprintFunctionLib_FocusUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @param ControllerRef: The reference to the Player controller (used to get viewport)
	 * @param WorldLocation: The world space to check if on Player screen
	 * @returns: Whether the location is on the particular Player's screen
	 */
	UFUNCTION(BlueprintPure, Category = "FocusUtils")
	static bool IsLocationOnPlayerScreen(const APlayerController* ControllerRef, const FVector WorldLocation);

	/**
	 * @param ControllerRef: The reference to the controller (used to get viewport)
	 * @param Location: The world space location
	 * @returns: The screen space angle from the Player to the location in world space (relative to a vector pointing straight up: FVector2D(0, -1))
	 */
	UFUNCTION(BlueprintPure, Category = "FocusUtils")
	static float GetPlayerToLocationViewportAngle(const APlayerController* ControllerRef, const FVector& Location);
	
	/**
	 * @param ControllerRef: The reference to the controller (used to get viewport)
	 * @param ActorRef: The reference to the AActor (that has a world space)
	 * @returns: The screen space angle from the Player to the AActor in world space
	 */
	UFUNCTION(BlueprintPure, Category = "FocusUtils")
	static float GetPlayerToActorAngle(const APlayerController* ControllerRef, const AActor* ActorRef);
	
	/**
	 * @param ControllerRef: The reference to the controller (used to get viewport)
	 * @param WidgetComponent: The reference to the WidgetComponent (that has a world space)
	 * @returns: The screen space angle from the Player to the WidgetComponent in world space
	 */
	UFUNCTION(BlueprintPure, Category = "FocusUtils")
	static float GetPlayerToWidgetComponentAngle(const APlayerController* ControllerRef, const UWidgetComponent* WidgetComponent);

	/**
	 * @param ViewportBounds: The bounds of the viewport rectangle (assumes that (0, 0) is top left corner) (will be used to calculate the center point)
	 * @param Angle: The angle of the line that is drawn from the center of the rectangle to one of the bounds (specifically the angle from GetPlayerToWidgetComponentAngle())
	 * @returns: The point on a rectangle's bounds that intersects with a line from the center of the rectangle, drawn by given angle
	 */
	UFUNCTION(BlueprintPure, Category = "FocusUtils")
	static FVector2D GetViewportBoundsIntersectionFromAngle(const FVector2D ViewportBounds, const float Angle);

	/**
	 * @param A_Start: The start of the first line segment
	 * @param A_End: The end of the first line segment
	 * @param B_Start: The start of the second line segment
	 * @param B_End: The end of the second line segment
	 * @param IntersectionPoint: The intersection point between the two line segments
	 * @returns: Whether an intersection exists between the two line segments
	 */
	UFUNCTION(BLueprintPure, Category = "FocusUtils")
	static bool GetIntersectionFromTwoLineSegments(const FVector2D A_Start, const FVector2D A_End, const FVector2D B_Start, const FVector2D B_End, FVector2D& IntersectionPoint);
};
