// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_PlayerHUD.h"

#include "AbilitySystemComponent.h"
#include "FocusfireCharacter.h"
#include "Components/TextBlock.h"

void UUserWidget_PlayerHUD::initializePlayerHUD(const AFocusfireCharacter* OwningFocusfireCharacter)
{
	PlayerAbilitySystemComponent = OwningFocusfireCharacter->GetAbilitySystemComponent();
	if (PlayerAbilitySystemComponent)
	{
		PlayerAbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(DuringFocusPeriodTag.GetTagName()), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UUserWidget_PlayerHUD::OnFocusPeriodTagChanged);
		PlayerAbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(DuringPingTag.GetTagName()), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UUserWidget_PlayerHUD::OnPingTagChanged);
	}
}

void UUserWidget_PlayerHUD::AddTextToTextLayoutWidgetByReason(UTextBlock* TextWidget,
	const FText& NewText, const ETextReason NewTextReason)
{
	// If this Text widget never had text added to it, then add it as a key to the map
	if (not Map_TextWidget_to_TextStack.Contains(TextWidget))
	{
		// Create TPair and add it to new array
		TPair<ETextReason, FText> _DefaultReason_to_DefaultText(ETextReason::DEFAULT, NewText);
		TPair<ETextReason, FText> _Reason_to_Text(NewTextReason, NewText);
		TArray<TPair<ETextReason, FText>> _list_Reason_to_Text;
		_list_Reason_to_Text.Add(_DefaultReason_to_DefaultText); // Add a default reason + text, that ideally won't ever be removed
		_list_Reason_to_Text.Add(_Reason_to_Text);
		
		Map_TextWidget_to_TextStack.Add(TextWidget, _list_Reason_to_Text);
	}

	// If the Text widget has text added to it, then see if we either:
	// * (Reason EXISTS) Modify text of old reason already on stack
	// * (Reason does NOT EXIST) Add a new reason and text to stack
	else
	{
		// Find index of reason, if it exists
		TArray<TPair<ETextReason, FText>>* _ptr_list_Reason_to_Text = &Map_TextWidget_to_TextStack[TextWidget];
		int32 _index_CurrentReason = _ptr_list_Reason_to_Text->IndexOfByPredicate([&NewTextReason]
			(const TPair<ETextReason, FText>& _Reason_to_Text)
			{
				return _Reason_to_Text.Key == NewTextReason;
			}
		);

		// Reason already exists in stack, modify current text
		if (_index_CurrentReason > -1)
		{
			(*_ptr_list_Reason_to_Text)[_index_CurrentReason].Value = NewText;
		}

		// Reason does not exist in stack, add it to the top of the stack
		else
		{
			TPair<ETextReason, FText> _Reason_to_Text(NewTextReason, NewText);
			_ptr_list_Reason_to_Text->Emplace(_Reason_to_Text);
		}
	}

	// Set the text of the Text widget
	TextWidget->SetText(NewText);

	// Debugging
	UE_LOG(LogTemp, Warning, TEXT("-------------------"));
	for (auto& _text_to_stack : Map_TextWidget_to_TextStack)
	{
		TArray<ETextReason> _reasons;
		for (auto& _reason_to_text : _text_to_stack.Value)
		{
			_reasons.Emplace(_reason_to_text.Key);
		}
		UE_LOG(LogTemp, Warning, TEXT("ccc * %s"), *_text_to_stack.Key->GetName());
		for (ETextReason _reason : _reasons) {
			UE_LOG(LogTemp, Warning, TEXT("* %s"), *UEnum::GetValueAsString(_reason));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("-------------------"));
}

void UUserWidget_PlayerHUD::RemoveTextToTextLayoutWidgetByReason(UTextBlock* TextWidget,
	const ETextReason RemoveTextReason)
{
	// If this Text widget never had text added to it, then do nothing
	if (not Map_TextWidget_to_TextStack.Contains(TextWidget))
	{
		return;
	}

	// Find index of reason, if it exists
	TArray<TPair<ETextReason, FText>>* _ptr_list_Reason_to_Text = &Map_TextWidget_to_TextStack[TextWidget];
	int32 _index_CurrentReason = _ptr_list_Reason_to_Text->IndexOfByPredicate([&RemoveTextReason]
		(const TPair<ETextReason, FText>& _Reason_to_Text)
		{
			return _Reason_to_Text.Key == RemoveTextReason;
		}
	);

	// Reason already exists in stack, modify current text
	if (_index_CurrentReason > -1)
	{
		TPair<ETextReason, FText> _Reason_to_Text = _ptr_list_Reason_to_Text->Pop();
		while (_Reason_to_Text.Key != RemoveTextReason)
		{
			_Reason_to_Text = _ptr_list_Reason_to_Text->Pop(); // Pop (remove elements from top of stack), until reaching matching reason
		}
		
		// Once matching reason has been popped, set the text to the top of the stack
		_Reason_to_Text = _ptr_list_Reason_to_Text->Last();
		TextWidget->SetText(_Reason_to_Text.Value);
	}
}

void UUserWidget_PlayerHUD::OnFocusPeriodTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	ToggleFocusPeriodHUDElements(NewCount > 0);
}

void UUserWidget_PlayerHUD::OnPingTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	TogglePingHUDElements(NewCount > 0);
}
