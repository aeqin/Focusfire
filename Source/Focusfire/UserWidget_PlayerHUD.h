// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_PlayerHUD.generated.h"

class UTextBlock;
class UTextLayoutWidget;
class UAbilitySystemComponent;
class AFocusfireCharacter;

UENUM(BlueprintType)
enum class ETextReason : uint8 // Used to keep track of Text stack
{
	DEFAULT,
	FOCUS_PERIOD,
	PING,
	FOCUS_DASH,
	FOCUS_LOCK,
};

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UUserWidget_PlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Called when on AFocusfireCharacter::BeginPlay() in order to set reference
	 * @param OwningFocusfireCharacter: The Player that this HUD should reference
	 */
	UFUNCTION()
	void initializePlayerHUD(const AFocusfireCharacter* OwningFocusfireCharacter);
	
	/**
	 * Event called when the Player's currently selected focus changes
	 * @param FocusStringToDisplay: What text to display (name of selected FocusBase) 
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="PlayerHUD")
	void OnCurrentlySelectedFocusChanged(const FString& FocusStringToDisplay);

	/**
	 * Event called when the Player is able to cast "GameplayAbility.Focus.Dash"
	 * @param bCanFocusDash: Is Player able to cast "GameplayAbility.Focus.Dash"?
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="PlayerHUD")
	void OnFocusDashisPossible(const bool bCanFocusDash);
	
	/**
	 * Event called when the Player enters/exits "GameplayAbility.Focus.Period" state
	 * @param bMakeVisible: Whether to display or hide UI elements
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="PlayerHUD")
	void ToggleFocusPeriodHUDElements(const bool bMakeVisible);

	/**
	 * Event called when the Player enters/exits "GameplayAbility.Ping" state
	 * @param bMakeVisible: Whether to display or hide UI elements
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="PlayerHUD")
	void TogglePingHUDElements(const bool bMakeVisible);

	/**
	 * Event called when the Player enters/exits "GameplayAbility.Ping" state
	 * @param bMakeVisible: Whether to display or hide UI elements
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="PlayerHUD")
	void ToggleFocusLockedHUDElements(const bool bMakeVisible);

protected: /** Text Stack */
	/**
	 * A map structure where each element of a Text widget refers to an array of its text.
	 * This array is in a Pair structure, so with the key being the "reason" for the changed text.
	 * Changing the text of a text widget adds to its array,
	 * while removing the text of a text widget removes the latest of its array (and everything later than it).
	 */
	TMap<UTextBlock*, TArray<TPair<ETextReason, FText>>> Map_TextWidget_to_TextStack;

	/**
	 * Change the text of a Text widget, preserve the action by adding it to the top of the Stack_Text
	 * @param TextWidget: The Text widget to change the text of
	 * @param NewText: The new text to be changed to
	 * @param NewTextReason: Why was the text changed
	 */
	UFUNCTION(BlueprintCallable, Category="PlayerHUD")
	void AddTextToTextLayoutWidgetByReason(UTextBlock* TextWidget, const FText& NewText, const ETextReason NewTextReason);

	/**
	 * Remove the text of a Text widget by reason, reverting to the text of a prior stack element
	 * @param TextWidget: The Text widget to change the text of
	 * @param RemoveTextReason: Why was the text changed
	 */
	UFUNCTION(BlueprintCallable, Category="PlayerHUD")
	void RemoveTextToTextLayoutWidgetByReason(UTextBlock* TextWidget, const ETextReason RemoveTextReason);
	
protected:
	/** The reference to the AbilitySystemComponent of the Player (to watch for GameplayTag changes) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PlayerHUD")
	UAbilitySystemComponent* PlayerAbilitySystemComponent;

	/** The GameplayTag that denotes if the Player is currently in the "GameplayAbility.Focus.Period" state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerHUD")
	FGameplayTag DuringFocusPeriodTag;

	/** The GameplayTag that denotes if the Player is currently in the "GameplayAbility.Ping" state */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PlayerHUD")
	FGameplayTag DuringPingTag;

	/** The event that fires when the Player enters or leaves the "GameplayAbility.Focus.Period" state */
	UFUNCTION()
	void OnFocusPeriodTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	/** The event that fires when the Player enters or leaves the "GameplayAbility.Ping" state */
	UFUNCTION()
	void OnPingTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
