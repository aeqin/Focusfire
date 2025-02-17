// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_FocusDash.generated.h"

class AFocusBase;

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UGameplayAbility_FocusDash : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	/** How close to dash to a FocusBase (stop this distance away) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusDash")
	float DashToOffset = 200.0f;

	/** How long does it take to dash to a FocusBase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusDash")
	float DashToDuration = 0.2f;
	
	/** Get a FocusBase to dash to */
	UFUNCTION(BlueprintCallable, Category = "FocusDash")
	AFocusBase* DashToFocusInRange();

protected: /* Overloads */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
};
