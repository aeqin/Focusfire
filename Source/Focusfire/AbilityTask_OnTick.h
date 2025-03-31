// Fill out your copyright notice in the Description page of Project Settings.
// https://dev.epicgames.com/community/snippets/dvJ/unreal-engine-on-tick-ability-task

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_OnTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTaskDelegate, float, DeltaTime);

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UAbilityTask_OnTick : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnTickTaskDelegate OnTick;

public:
	UAbilityTask_OnTick(const FObjectInitializer& ObjectInitializer);
	 
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_OnTick* AbilityTaskOnTick(
		UGameplayAbility* OwningAbility,
		FName TaskInstanceName);
		
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
};
