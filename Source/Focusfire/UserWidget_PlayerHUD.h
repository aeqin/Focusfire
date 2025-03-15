// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UUserWidget_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Event called when the Player's currently selected focus changes
	 * @param FocusStringToDisplay: What text to display (name of selected FocusBase) 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="PlayerHUD")
	void OnCurrentlySelectedFocusChanged(const FString& FocusStringToDisplay);
};
