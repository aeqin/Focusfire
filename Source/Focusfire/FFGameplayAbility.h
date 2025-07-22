// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FFGameplayAbility.generated.h"

class UActorComponent_ManagerFocus;
/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UFFGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	/** 
	* Gets the default asset GameplayTags that this GameplayAbility has
	*/
	UFUNCTION(BlueprintCallable, Category = "FF_GameplayAbility")
	FGameplayTagContainer GetGameplayAbilityAssetTags() { return GetAssetTags(); };

	/** 
	* Gets the default asset GameplayTag that this GameplayAbility has (the first tag in its asset tag container)
	*/
	UFUNCTION(BlueprintCallable, Category = "FF_GameplayAbility")
	FGameplayTag GetDefaultGameplayAbilityAssetTag() { return GetAssetTags().First(); };

	/** 
	* Gets the Focus Manager for focus-related functions
	*/
	UFUNCTION(BlueprintPure, Category = "FF_GameplayAbility")
	UActorComponent_ManagerFocus* GetFocusManager();

	// The reference to the Focus Manager (which contains useful focus-related functions)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FF_GameplayAbility")
	TObjectPtr<UActorComponent_ManagerFocus> ManagerFocus;
};
