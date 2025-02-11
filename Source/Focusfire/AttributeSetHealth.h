// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "AttributeSetHealth.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UAttributeSetHealth : public UAttributeSetBase
{
	GENERATED_BODY()

public: /* Properties */
	UPROPERTY()
	FAttributeAfterEffectEvent OnHealthChanged;
	 
	UPROPERTY(BlueprintReadWrite, Category = "AttributeSet")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetHealth, Health);

public: /* Functions */
	/** 
	* Overriding PostGameplayEffectExecute to broadcast FAttributeAfterEffectEvent after the Health value is changed
	* @param Data struct FGameplayEffectModCallbackData
	* @return
	*/
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
