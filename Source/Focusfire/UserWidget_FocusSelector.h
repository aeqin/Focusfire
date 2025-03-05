// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_FocusSelector.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UUserWidget_FocusSelector : public UUserWidget
{
	GENERATED_BODY()

	/** Material instance that draws the radial menu **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* RadialMenuMaterial;
	
	/** Spawned Dynamic Material instance that draws the radial menu **/
	UPROPERTY(BlueprintReadWrite, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	UMaterialInstanceDynamic* RadialMenuDynamic;

	/** The current amount of slices in the radial menu **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	int NumSlices;

	/** The angle size of each individual slice **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	int SliceAngleSize;

	/** 
	* Calculates the current angle of the mouse position in respect to the radial menu
	* @return The current mouse angle
	*/
	UFUNCTION(BlueprintPure, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	float CalcCurrentAngle();
};
