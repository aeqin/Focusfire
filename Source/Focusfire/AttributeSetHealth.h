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
	UPROPERTY(BlueprintReadWrite, Category = "AttributeSet")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetHealth, Health);

	UPROPERTY(BlueprintReadWrite, Category = "AttributeSet")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetHealth, MaxHealth);
public: /* Functions */
	/** 
	* Overriding PreAttributeChange to clamp health values
	* @param Attribute The specific attribute being set
	* @param NewValue The new value to potentially set attribute to
	* @return
	*/
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
};
