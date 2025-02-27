// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FocusBase.h"
#include "FocusBaseRebound.generated.h"

/**
 * 
 */
UCLASS()
class FOCUSFIRE_API AFocusBaseRebound : public AFocusBase
{
	GENERATED_BODY()

protected:
	/** The ratio at which this FocusBaseRebound is redirected when its ability is activated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusBaseRebound")
	float ReboundRatio = 2.0;

	/** The number of times this FocusBaseRebound has been re-directed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusBaseRebound")
	int NumBounces = 0;

	/** How many times this FocusBaseRebound may be re-directed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusBaseRebound")
	int MaxBounces = 2;
	
public:
	/** 
	* Activate the special ability of this FocusBaseRebound.
	* Shoot the FocusBase in another direction faster & stronger
	* @param Activator: Used to shoot the FocusBaseRebound in the direction that the Activator is facing
	*/
	virtual void ActivateAbility(const AActor* Activator) override;
};
