// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFGameplayAbility.h"
#include "FFGameplayAbility_FocusHop.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UFFGameplayAbility_FocusHop : public UFFGameplayAbility
{
	GENERATED_BODY()
	
protected:
	/** How far to hop */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusDash")
	float HopDistance = 100.0f;

	/** How long to hop */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusDash")
	float HopDuration = 0.2f;

protected: /* Overloads */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
};
