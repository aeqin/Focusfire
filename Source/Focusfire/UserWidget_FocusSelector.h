// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFConstants_Struct.h"
#include "Blueprint/UserWidget.h"
#include "FocusBase.h"
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
	float SliceAngleSize;

	/** The possible FocusBase selection slices **/
	UPROPERTY(BlueprintReadWrite, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AFocusBase>> FocusSelections;

	/** The possible FocusBase selection slices **/
	UPROPERTY(BlueprintReadWrite, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	TArray<EFocusType> List_SelectableFocuses;

	/** The selected FocusBase (right before closing the menu) **/
	UPROPERTY(BlueprintReadWrite, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	EFocusType CurrentSelectedFocusType = EFocusType::INVALID;
	
	/** The selected FocusBase (right before closing the menu) **/
	UPROPERTY(BlueprintReadWrite, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AFocusBase> CurrentlySelectedShoot = nullptr;
	
	/** 
	* Calculates the current angle of the mouse position in respect to the radial menu
	* @return False if there's no player HUD
	*/
	UFUNCTION(BlueprintPure, Category = "FocusSelector", meta = (AllowPrivateAccess = "true"))
	bool CalcCurrentRadialAngle(float& radialAngle, float& radialDistance);

public:
	/**
	 * Get the last selected FocusBase in the radial menu
	 * @return The type of FocusBase that was last selected by the radial menu
	 */
	UFUNCTION(BlueprintPure, Category = "FocusSelector")
	FORCEINLINE EFocusType GetCurrentSelectedFocusType() const { return CurrentSelectedFocusType; }

	/**
	 * Adds a new slice (representing a FocusBase) to the radial menu
	 * @param FocusType: The type of FocusBase to be added on the radial menu
	 */
	UFUNCTION(BlueprintCallable, Category = "FocusSelector")
	void AddFocusTypeToMenu(EFocusType FocusType);

	/** 
	* Event that is fired when a new slice is added to the radial menu
	* @param FocusType: The type of FocusBase to be added on the radial menu
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusSelector")
	void OnNewFocusTypeSliceAdded(EFocusType FocusType);
};
