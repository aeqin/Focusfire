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

public:
	/** 
	* Activate the special ability of this FocusBaseRebound.
	* Shoot the FocusBase in another direction faster & stronger
	*/
	virtual void ActivateAbility() override;
};
