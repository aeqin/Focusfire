// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FocusBase.h"
#include "FocusBaseLaunch.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API AFocusBaseLaunch : public AFocusBase
{
	GENERATED_BODY()

protected:
	/** How fast should activator be launched when ability is activated */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBaseRebound")
	float LaunchSpeed = 2000;

	/** The angle that the activator is launched when ability is activated */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBaseRebound")
	float LaunchAngle = -30;

public:
	/** 
	* Activate the special ability of this FocusBaseLaunch.
	* Launch the activator in their forward direction
	* @param Activator: Used to get the direction that the Activator is facing
	*/
	virtual void ActivateAbility(const AActor* Activator) override;
};
