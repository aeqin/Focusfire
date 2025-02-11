// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetHealth.h"
#include "GameplayEffectExtension.h"

void UAttributeSetHealth::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		OnHealthChanged.Broadcast(Data.EvaluatedData.Magnitude, GetHealth());
	}
}
