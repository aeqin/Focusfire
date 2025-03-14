// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFunctionLib_FocusUtils.h"

#include "FocusfireCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool UBlueprintFunctionLib_FocusUtils::IsLocationOnPlayerScreen(const APlayerController* ControllerRef, const FVector WorldLocation)
{
	FVector2D _ScreenLocation;
	int _ViewportSizeX;
	int _ViewportSizeY;
	
	if (not ControllerRef->ProjectWorldLocationToScreen(WorldLocation, _ScreenLocation))
		return false; // Failed to project to screen
	
	ControllerRef->GetViewportSize(_ViewportSizeX, _ViewportSizeY);

	if (_ScreenLocation.X < 0 || _ScreenLocation.X > _ViewportSizeX) return false; // Off left or right edge of screen

	if (_ScreenLocation.Y < 0 || _ScreenLocation.Y > _ViewportSizeY) return false; // Off top or bottom edge of screen
	
	return true; // On screen
}

float UBlueprintFunctionLib_FocusUtils::GetPlayerToLocationViewportAngle(const APlayerController* ControllerRef,
	const FVector& Location)
{
	const FVector CameraLocation = ControllerRef->PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = ControllerRef->PlayerCameraManager->GetCameraRotation();

	const FVector CameraForwardVector = UKismetMathLibrary::GetForwardVector(CameraRotation);
	const FVector CameraRightVector = UKismetMathLibrary::GetRightVector(CameraRotation);
	const FVector CameraUpVector = UKismetMathLibrary::GetUpVector(CameraRotation);

	const FVector ProjectedVector = UKismetMathLibrary::ProjectVectorOnToPlane(Location - CameraLocation, CameraForwardVector).GetSafeNormal();

	// Calculate angle
	const float Dot = FVector::DotProduct(ProjectedVector, CameraRightVector);
	float Angle;
	if (FVector::DotProduct(ProjectedVector, CameraUpVector) > 0.0f) // Top half of screen
	{
		Angle = -1 * UKismetMathLibrary::DegAcos(Dot); // Need to flip sign, otherwise top half points down (same as bottom half)
	}
	else // Bottom half of screen
	{
		Angle = UKismetMathLibrary::DegAcos(Dot);
	}
	return Angle + 90; // Add 90 so that Angle is in relation to a vector pointing straight up FVector2D(0, -1)
}

float UBlueprintFunctionLib_FocusUtils::GetPlayerToActorAngle(const APlayerController* ControllerRef,
                                                              const AActor* ActorRef)
{
	return GetPlayerToLocationViewportAngle(ControllerRef, ActorRef->GetActorLocation());
}

float UBlueprintFunctionLib_FocusUtils::GetPlayerToWidgetComponentAngle(const APlayerController* ControllerRef,
                                                                        const UWidgetComponent* WidgetComponent)
{
	return GetPlayerToLocationViewportAngle(ControllerRef, WidgetComponent->GetComponentLocation());

	// Old implementation
	// FVector _PlayerLocation = ControllerRef->GetPawn()->GetActorLocation();
	// FVector _WidgetLocation = WidgetComponent->GetComponentLocation();
	// FRotator _LookAtRotator = (_PlayerLocation - _WidgetLocation).Rotation();
	// float _AngleFromPlayerViewToWidgetComponent = _LookAtRotator.Yaw - ControllerRef->GetControlRotation().Yaw;
	//
	// return _AngleFromPlayerViewToWidgetComponent;
}

FVector2D UBlueprintFunctionLib_FocusUtils::GetViewportBoundsIntersectionFromAngle(const FVector2D ViewportBounds,
	const float Angle)
{
	// Center point of the viewport
	FVector2D _CenterPoint = ViewportBounds / 2.0f;

	// Max line length would be the distance from center of rectangle to corner
	float _MaxLineLength = (FVector2D(_CenterPoint.X, 0) - FVector2D(0, _CenterPoint.Y)).Length();

	// Old implementation
	/* Calculate the line angle from the GetPlayerToWidgetComponentAngle() angle
	float _LineAngle;
	if (Angle <= 0)
	{
		_LineAngle = 180 - FMath::Abs(Angle);
	}
	else
	{
		_LineAngle = Angle + 180;
	}*/

	// Calculate end point of line from angle (using unit vector pointing up)
	FVector2D _LineEndPoint = _CenterPoint + FVector2D(0, -1).GetRotated(Angle) * _MaxLineLength;

	// Calculate intersection of line with rectangle bounds
	FVector2D _IntersectionPoint;

	// Check intersection with left of viewport
	if (GetIntersectionFromTwoLineSegments(
		FVector2D(0, 0), FVector2D(0, ViewportBounds.Y),
		_CenterPoint, _LineEndPoint,
		_IntersectionPoint))
	{
		return _IntersectionPoint;
	}

	// Check intersection with right of viewport
	if (GetIntersectionFromTwoLineSegments(
		FVector2D(ViewportBounds.X, 0), FVector2D(ViewportBounds.X, ViewportBounds.Y),
		_CenterPoint, _LineEndPoint,
		_IntersectionPoint))
	{
		return _IntersectionPoint;
	}

	// Check intersection with top of viewport
	if (GetIntersectionFromTwoLineSegments(
		FVector2D(0, 0), FVector2D(ViewportBounds.X, 0),
		_CenterPoint, _LineEndPoint,
		_IntersectionPoint))
	{
		return _IntersectionPoint;
	}

	// Check intersection with bot of viewport
	if (GetIntersectionFromTwoLineSegments(
		FVector2D(0, ViewportBounds.Y), FVector2D(ViewportBounds.X, ViewportBounds.Y),
		_CenterPoint, _LineEndPoint,
		_IntersectionPoint))
	{
		return _IntersectionPoint;
	}
	
	return _IntersectionPoint;
}

bool UBlueprintFunctionLib_FocusUtils::GetIntersectionFromTwoLineSegments(const FVector2D A_Start,
	const FVector2D A_End, const FVector2D B_Start, const FVector2D B_End, FVector2D& IntersectionPoint)
{
	// Calculate rise and run of each line segment
	float _rise_A = A_End.Y - A_Start.Y;
	float _run_A = A_End.X - A_Start.X;
	float _rise_B = B_End.Y - B_Start.Y;
	float _run_B = B_End.X - B_Start.X;

	// Calculate alpha / beta, which is a [0, 1] "percentage" of points on a line
	float _alpha = (-_rise_A * (A_Start.X - B_Start.X) + _run_A * (A_Start.Y - B_Start.Y)) / (-_run_B * _rise_A + _run_A * _rise_B);
	float _beta = (_run_B * (A_Start.Y - B_Start.Y) - _rise_B * (A_Start.X - B_Start.X)) / (-_run_B * _rise_A + _run_A * _rise_B);

	// There is a collision, so set intersection point
	if (_alpha >= 0 && _alpha <= 1 && _beta >= 0 && _beta <= 1)
	{
		IntersectionPoint = FVector2D(A_Start.X + (_beta * _run_A), A_Start.Y + (_beta * _rise_A));
	
		return true;
	}
	else
	{
		return false;
	}
}

