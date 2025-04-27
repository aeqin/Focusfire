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

	/** 
	* Event that is fired to set an AActor's custom time dilation.
	* Blueprint will handle running on Server, then multicasting the slowdown to all Clients
	* @param ActorToTimeModify: The AActor whose time dilation is to be modified
	* @param NewTimeDilation: New custom time dilation to set
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "FocusfireGameState")
	void SetActorTimeDilationReplicated(AActor* ActorToTimeModify, float NewTimeDilation);
};
