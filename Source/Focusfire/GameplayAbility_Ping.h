// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_Ping.generated.h"

class APingSphere;
/**
 * 
 */
UCLASS()
class FOCUSFIRE_API UGameplayAbility_Ping : public UGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ping", meta = (AllowPrivateAccess = "true"))
	float PingMaxRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ping", meta = (AllowPrivateAccess = "true"))
	float PingCurrentRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ping", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APingSphere> SpawnedPingSphereClass;
	
	UPROPERTY(BlueprintReadWrite, Category="Ping", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<APingSphere> SpawnedPingSphere;
	
	/** 
	* Calculate initial range of ping
	*/
	UFUNCTION(BlueprintCallable, Category = "Ping")
	void PingStart();
};
