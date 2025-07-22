// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFGameplayAbility.h"
#include "FFGameplayAbility_FocusDash.generated.h"

class AFocusBase;

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UFFGameplayAbility_FocusDash : public UFFGameplayAbility
{
	GENERATED_BODY()

protected:
	/** Default how close to dash to a FocusBase (stop this distance away) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusDash")
	float DefaultDashToOffsetX = 100.0f;
	
	/** How close to dash to a FocusBase (stop this distance away) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusDash")
	float DashToOffsetX = 100.0f;

	/** How long does it take to dash to a FocusBase */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusDash")
	float DashToDuration = 0.2f;
	
	/** Gets a FocusBase to dash to, called just before dashing to it in BP using ApplyRootMotionMoveToActorForce task */
	UFUNCTION(BlueprintCallable, Category = "FocusDash")
	AFocusBase* DashToFocusInRange();

protected: /* Overloads */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
};
