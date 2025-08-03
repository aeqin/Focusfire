// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FFConstants_Enum.h"
#include "Components/ActorComponent.h"
#include "ActorComponent_ManagerFocus.generated.h"


struct FFStruct_FocusData;
enum class EFocusDirective : uint8;
enum class EFocusType : uint8;
class APingSphere;
class UWidgetComponent;
class UUserWidget_FocusMarker;
class AFocusBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FOCUSFIRE_API UActorComponent_ManagerFocus : public UActorComponent
{
	GENERATED_BODY()

	/** An int that guarantees that FocusBases have unique identifiers (decided by Server) */
	int Generator_CurrentValid_UID = 0;

	/** A list of FocusBase (spawned by Client, ONLY on Client), that has not synced with a server-replicated FocusBase */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TArray<AFocusBase*> List_UnSynced_ClientFocus;

	/** A list of server-replicated FocusBase that maps to non-replicated FocusBase this Client spawned for itself */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TMap<AFocusBase*, AFocusBase*> Map_ServerFocus_ClientFocus;
	
	/** A map of FocusBase to UUserWidget_FocusMarker currently active in the game */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ManagerFocus", meta = (AllowPrivateAccess = true))
	TMap<AFocusBase*, UUserWidget_FocusMarker*> Map_ClientFocus_Widget;

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
	void OnServerFocusDestroyed(AActor* DestroyedActor);

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

	// As FocusBase Manager, gain access to private/protected properties of FocusBase
	friend class AFocusBase; 

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * When a FocusBase is initialized, expect it to call ManagerFocus, and register itself
	 * @param NewFocus: Reference to the new Focus
	 */
	UFUNCTION(BlueprintCallable, Category = "ManagerFocus")
	void RegisterFocus(AFocusBase* NewFocus);

	/**
	 * Spawn a new FocusBase on this Client
	 * @param Spawner: Actor that spawned this FocusBase
	 * @param FocusSpawnData: Data that describes how the FocusBase should be spawned
	 * @param FocusTypeToSpawn: Type of focus to spawn
	 * @param SpawnLocation: World location to spawn focus
	 * @param SpawnDirective: Describes how the focus is spawned
	 * @param AfterSpawnVector: Only if SpawnType is not EFocusSpawnType::DEFAULT, describes how the focus is shot after spawning
	 * @returns: The Client-only newly created FocusBase
	 */
	UFUNCTION(BLueprintCallable, Category = "ManagerFocus")
	AFocusBase* SpawnFocusOnClientOnly(AActor* Spawner, const FFStruct_FocusData& FocusSpawnData);// EFocusType FocusTypeToSpawn, FVector SpawnLocation);//, EFocusSpawnDirective SpawnDirective, FVector AfterSpawnVector);

	/**
	 * Spawn a new FocusBase on this Client, then request Server to spawn a replicated FocusBase
	 * @param Spawner: Actor that spawned this FocusBase
	 * @param FocusSpawnData: Data that describes how the FocusBase should be spawned
	 * @param FocusTypeToSpawn: Type of focus to spawn
	 * @param SpawnLocation: World location to spawn focus
	 * @returns: The Client-only newly created FocusBase
	 */
	UFUNCTION(BLueprintCallable, Category = "ManagerFocus")
	AFocusBase* SpawnFocusOnClientOnly_Then_SpawnOnServer_RPC(AActor* Spawner, const FFStruct_FocusData& FocusSpawnData);
	
	/**
	 * Spawn a new FocusBase on the Server, which will be replicated to all other clients (called by Client)
	 * @param Spawner: Actor that spawned this FocusBase
	 * @param FocusSpawnData: Data that describes how the FocusBase should be spawned
	 */
	UFUNCTION(Server, Reliable, Category = "ManagerFocus")
	void RPC_Server_SpawnFocus(AActor* Spawner, const FFStruct_FocusData& FocusSpawnData);
	
	/**
	 * After Server-replicated FocusBase is directed, tell client to also direct the locally-predicted FocusBase
	 * @param ServerReplicatedFocus: Reference to the Server-replicated FocusBase (the invisible focus that the Player does NOT see)
	 * @param Directive: Contains info on how to direct the FocusBase
	 * @param DirectiveVector: Contains vector info to use with Directive parameter
	 * @param DirectiveFloat: Contains float info to use with Directive parameter
	 * 
	 */
	UFUNCTION(Client, Reliable, BLueprintCallable, Category = "ManagerFocus")
	void RPC_Client_DirectClientPredictedFocus(AFocusBase* ServerReplicatedFocus, const EFocusDirective& Directive, const FVector& DirectiveVector = FVector::ZeroVector, const float& DirectiveFloat = 0.0f);
	
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
