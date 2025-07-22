// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFunctionLib_FocusUtils.h"

#include "FFGameplayAbilityTargetData_FocusData.h"
#include "FocusfireCharacter.h"
#include "FocusfireGameState.h"
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

FGameplayAbilityTargetDataHandle UBlueprintFunctionLib_FocusUtils::CreateTargetData_FocusData(
	const FVector SpawnLocation, const FVector SpawnDirection, const EFocusType SpawnType)
{
	// Create our target data type, 
	// Handle's automatically cleanup and delete this data when the handle is destructed, 
	// if you don't add this to a handle then be careful because this deals with memory management and memory leaks so its safe to just always add it to a handle at some point in the frame!
	FFGameplayAbilityTargetData_FocusData* _FocusData = new FFGameplayAbilityTargetData_FocusData();
	
	// Setup the struct's information to use the inputted name and any other changes we may want to do
	_FocusData->FocusData = FFStruct_FocusData(SpawnLocation, SpawnDirection, SpawnType);
	
	// Make our handle wrapper for Blueprint usage
	FGameplayAbilityTargetDataHandle Handle;
	// Add the target data to our handle
	Handle.Add(_FocusData);
	// Output our handle to Blueprint
	return Handle;
}

FString UBlueprintFunctionLib_FocusUtils::GetStringFromFocusData(const FFStruct_FocusData FocusData)
{
	if (FocusData.IsValidStruct())
	{
		return FString::Printf(TEXT("SpawnLocation: [%s]\nSpawnDirection: [%s]\nSpawnType: [%s]"), *FocusData.SpawnLocation.ToString(), *FocusData.SpawnLocation.ToString(), *UEnum::GetValueAsString(FocusData.FocusType));
	}
	else
	{
		return TEXT("FocusData ERROR: Attempting to print uninitialized FocusData");
	}
}

FFStruct_FocusData UBlueprintFunctionLib_FocusUtils::GetFocusDataFromTargetDataHandle(
	const FGameplayAbilityTargetDataHandle& Handle, const int Index)
{
	if (Handle.Data.IsValidIndex(Index))
	{
		// Check data at index is valid
		FGameplayAbilityTargetData* _Data = Handle.Data[Index].Get();
		if(_Data == nullptr)
		{
			return FFStruct_FocusData();
		}

		// This is basically the type checking pass, static_cast does not have type safety, this is why we do this check.
		// If we don't do this then it will object slice the struct and thus we have no way of making sure its that type.
		if(_Data->GetScriptStruct() == FFGameplayAbilityTargetData_FocusData::StaticStruct())
		{
			// Here is when you would do the cast because we know its the correct type already
			FFGameplayAbilityTargetData_FocusData* _FocusData = static_cast<FFGameplayAbilityTargetData_FocusData*>(_Data);
			return FFStruct_FocusData(_FocusData->FocusData.SpawnLocation, _FocusData->FocusData.SpawnDirection, _FocusData->FocusData.FocusType);
		}
	}
	
	return FFStruct_FocusData();
}

