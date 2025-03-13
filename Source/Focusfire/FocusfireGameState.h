// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FocusfireGameState.generated.h"

class UActorComponent_ManagerFocus;
/**
 * 
 */
UCLASS()
class FOCUSFIRE_API AFocusfireGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FocusfireGameState", meta = (AllowPrivateAccess = "true"))
	UActorComponent_ManagerFocus* Manager_Focus;
	
public:
	AFocusfireGameState();

	/**
	 * @return The Manager that handles FocusBase
	 */
	UFUNCTION(BlueprintCallable, Category = "FocusfireGameState")
	FORCEINLINE UActorComponent_ManagerFocus* GetManagerFocus() const { return Manager_Focus; }
};
