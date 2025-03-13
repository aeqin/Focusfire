// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorComponent_ManagerFocus.generated.h"


class UUserWidget_FocusMarker;
class AFocusBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOCUSFIRE_API UActorComponent_ManagerFocus : public UActorComponent
{
	GENERATED_BODY()

	/** A map of FocusBase to UUserWidget_FocusMarker currently active in the game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TMap<AFocusBase*, UUserWidget_FocusMarker*> Map_Focus_Widget;

	/** The class of widget to spawn (as offscreen indicator) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> FocusMarkerWidgetClass;
	
	/**
	 * Each frame, position a widget that points towards each FocusBase off the screen of the Player
	 */
	UFUNCTION()
	void OnTickPositionOffscreenIndicators();
	
	/** Function that is fired when a FocusBase is destroyed */
	UFUNCTION()
	void OnFocusDestroyed(AActor* DestroyedActor);
	
public:	
	// Sets default values for this component's properties
	UActorComponent_ManagerFocus();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Spawn a FocusBase at the specified location, then shoot it forward
	 * @param SpawnTransform: The location in world space to spawn the FocusBase
	 * @param ShootDirection: The direction to shoot the FocusBase (FocusBase itself handles speed)
	 * @param FocusTypeToSpawn: The type of FocusBase to spawn (such as FocusBaseRebound)
	 * @param Spawner: The AActor that spawned this FocusBase
	 */
	UFUNCTION(BlueprintCallable, Category = "ManagerFocus")
	void ShootFocusInDirection(FTransform SpawnTransform, FVector ShootDirection, TSubclassOf<class AFocusBase> FocusTypeToSpawn, AActor* Spawner);

		
};
