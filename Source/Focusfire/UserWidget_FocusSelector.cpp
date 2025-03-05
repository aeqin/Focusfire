// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_FocusSelector.h"

float UUserWidget_FocusSelector::CalcCurrentAngle()
{
	// Get Mouse position vector
	FVector _mousePos = FVector();
	GetOwningPlayer()->GetMousePosition(_mousePos.X, _mousePos.Y);

	// Get center of viewport position vector
	int _viewX;
	int _viewY;
	GetOwningPlayer()->GetViewportSize(_viewX, _viewY);
	FVector _viewportSize = FVector(_viewX, _viewY, 0.0f) / 2.0;

	// Calculate look at rotation
	FRotator _mouseRotation = (_viewportSize - _mousePos).Rotation();
	
	return 180 - _mouseRotation.Yaw;
}
