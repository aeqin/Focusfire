// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetHealth.h"
#include "GameplayEffectExtension.h"

void UAttributeSetHealth::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetHealthAttribute())
	{
		// Clamp Health between 0 and max Health
		NewValue = FMath::Clamp<float>(NewValue, 0, GetMaxHealth());
	}
}
