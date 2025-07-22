// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_FocusSelector.h"

bool UUserWidget_FocusSelector::CalcCurrentRadialAngle(float& radialAngle, float& radialDistance)
{
	// Get Mouse position vector
	FVector _mousePos = FVector();
	if (not GetOwningPlayer()->GetMousePosition(_mousePos.X, _mousePos.Y))
	{
		return false;
	}

	// Get center of viewport position vector
	int _viewX;
	int _viewY;
	GetOwningPlayer()->GetViewportSize(_viewX, _viewY);
	FVector _viewportSize = FVector(_viewX, _viewY, 0.0f) / 2.0;

	// Calculate look at rotation
	FVector _offset = (_viewportSize - _mousePos);
	FRotator _mouseRotation = _offset.Rotation();
	
	radialAngle = 180 - _mouseRotation.Yaw; // Angle of mouse in respect to circle
	radialDistance = _offset.Length(); // Distance of mouse from center of circle
	
	return true;
}

void UUserWidget_FocusSelector::AddFocusTypeToMenu(EFocusType FocusType)
{
	if (not List_SelectableFocuses.Contains(FocusType))
	{
		List_SelectableFocuses.Add(FocusType);
		OnNewFocusTypeSliceAdded(FocusType); // Signal BP to modify the dynamic material that draws the radial menu
	}
}
