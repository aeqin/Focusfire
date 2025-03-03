// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FocusBase.h"
#include "FocusBaseMeteor.generated.h"

class UGameplayAbility;
/**
 * 
 */
UCLASS()
class FOCUSFIRE_API AFocusBaseMeteor : public AFocusBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusBaseMeteor")
	TSubclassOf<UGameplayAbility> GameplayAbility_FocusMeteor;
	
public:
	/** 
	* Activate the special ability of this FocusBaseMeteor.
	* Make the Activator slam down from current position, and deal area damage upon landing
	* @param Activator: Reference to move
	*/
	virtual void ActivateAbility(const AActor* Activator) override;
};
