// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorComponent_ManagerFocus.generated.h"


enum class EFocusType : uint8;
class APingSphere;
class UWidgetComponent;
class UUserWidget_FocusMarker;
class AFocusBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOCUSFIRE_API UActorComponent_ManagerFocus : public UActorComponent
{
	GENERATED_BODY()

	/** A map of FocusBase to UUserWidget_FocusMarker currently active in the game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TMap<AFocusBase*, UUserWidget_FocusMarker*> Map_Focus_Widget;

	/** A map of PingSphere to UUserWidget_FocusMarker currently active in the game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TMap<APingSphere*, UUserWidget_FocusMarker*> Map_Ping_Widget;

	/** The class of widget to spawn (as offscreen indicator) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> FocusMarkerWidgetClass;
	
	/**
	 * Each frame, position a widget that points towards each FocusBase off the screen of the Player
	 */
	UFUNCTION()
	void OnTickPositionOffscreenIndicators();

	/**
	 * Helper function for OnTickPositionOffscreenIndicators(), each frame, position a widget that points towards each FocusBase off the screen of the Player
	 * @param PlayerController: Reference to current Player
	 * @param WidgetComponent: Reference to the WidgetComponent (that has a world position) that contains the widget
	 * @param MarkerWidget: Reference to the widget of the FocusBase
	 */
	UFUNCTION()
	static void PositionOffscreenIndicators(const APlayerController* PlayerController, const UWidgetComponent* WidgetComponent, UUserWidget_FocusMarker* MarkerWidget);
	
	/** Function that is fired when a FocusBase is destroyed */
	UFUNCTION()
	void OnFocusDestroyed(AActor* DestroyedActor);

	/** Function that is fired when a PingSphere is destroyed */
	UFUNCTION()
	void OnPingDestroyed(AActor* DestroyedActor);

	/** A map of focus type to Color that best represents it */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TMap<EFocusType, FLinearColor> Map_FocusType_Color;

	/** A map of focus type to String that best represents it */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TMap<EFocusType, FString> Map_FocusType_String;

	/** A map of focus type to Class that best represents it (actor class that would be spawned) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TMap<EFocusType, TSubclassOf<AFocusBase>> Map_FocusType_FocusClass;
	
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
	AFocusBase* ShootFocusInDirection(FTransform SpawnTransform, FVector ShootDirection, TSubclassOf<class AFocusBase> FocusTypeToSpawn, AActor* Spawner);

	/**
	 * Spawn a FocusBase at the specified location, then shoot it forward
	 * @param SpawnTransform: The location in world space to spawn the FocusBase
	 * @param LockLocation: The location that the FocusBase will move to
	 * @param FocusTypeToSpawn: The type of FocusBase to spawn (such as FocusBaseRebound)
	 * @param Spawner: The AActor that spawned this FocusBase
	 */
	UFUNCTION(BlueprintCallable, Category = "ManagerFocus")
	AFocusBase* ShootFocusToLocation(FTransform SpawnTransform, FVector LockLocation, TSubclassOf<class AFocusBase> FocusTypeToSpawn, AActor* Spawner);
	
	/**
	 * Spawn a PingSphere at the specified location
	 * @param SpawnTransform: The location in world space to spawn the PingSphere
	 * @param PingClassToSpawn: The type of PingSphere to spawn
	 * @param Spawner: The AActor that spawned this PingSphere
	 */
	UFUNCTION(BlueprintCallable, Category = "ManagerFocus")
	APingSphere* SpawnPing(FTransform SpawnTransform, TSubclassOf<class APingSphere> PingClassToSpawn, AActor* Spawner);
	
	/**
	 * Get the color that best represents the Focus
	 * @param FocusType: The type of the focus to check
	 */
	UFUNCTION(BlueprintCallable, Category = "ManagerFocus")
	FLinearColor GetFocusColor(EFocusType FocusType);
	
	/**
	 * Get the String that best represents the Focus
	 * @param FocusType: The type of the focus to check
	 */
	UFUNCTION(BlueprintCallable, Category = "ManagerFocus")
	FString GetFocusString(EFocusType FocusType);

	/**
	 * Get the FocusBase child class that best represents the focus type
	 * @param FocusType: The type of the focus to check
	 */
	UFUNCTION(BlueprintCallable, Category = "ManagerFocus")
	TSubclassOf<AFocusBase> GetFocusClass(EFocusType FocusType);
};
