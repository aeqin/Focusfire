// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FocusPeriodSlowZone.generated.h"

class USphereComponent;

UCLASS()
class FOCUSFIRE_API AFocusPeriodSlowZone : public AActor
{
	GENERATED_BODY()

	/** The zone around the "GameplayAbility.Focus.Period" activator that slows down FocusBase and FocusfireCharacter */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusPeriodSlowZone", meta = (AllowPrivateAccess = "true"))
	USphereComponent* c_SphereOfSlowEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FCollisionProfileName CollisionProfile = FName("FocusPeriodZone");

	/** The set of AActors that are currently being slowed by the FocusPeriodSlowZone */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSet<AActor*> Set_SlowdownActors;

	/** The set of AActors that are ignored from slowdown by the FocusPeriodSlowZone */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSet<AActor*> Set_IgnoreSlowdownActors;

public:	
	// Sets default values for this actor's properties
	AFocusPeriodSlowZone();

	/** The radius of the sphere where the FocusPeriodZone slows down the time of AActors */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusPeriodSlowZone")
	float RadiusOfSlowEffect = 2000.0f;

	/** The time dilation to slow down affected AActors by */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocusPeriodSlowZone")
	float StrengthOfTimeSlowdown = 0.1f;

	/**
	 * Either add / remove an Actor to ignore for the time slow down
	 * @param ActorToIgnore: A pointer to the AActor that should not be slowed down
	 * @param bDoAddException: Whether or not the particular actor should not be slowed down, or if previously excepted, slowed down
	 */
	UFUNCTION()
	void SetActorSlowdownException(AActor* ActorToIgnore, bool bDoAddException);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** 
	* Called in response to the OnComponentBeginOverlap event
	* @param OverlappedComponent: A pointer to the component that triggered the event (i.e., the component of the actor receiving the event).
	* @param OtherActor:  A pointer to the actor that overlapped with the component.
	* @param OtherComp: A pointer to the component of the other actor that overlapped with the component.
	* @param OtherBodyIndex: The index of the body (in case of skeletal meshes or complex collision) of the other component that overlapped.
	* @param bFromSweep: Indicates whether the overlap was the result of a sweep (movement with collision detection) or not.
	* @param SweepResult: If bFromSweep is true, this contains detailed information about the sweep hit. Otherwise, it will contain default values.
	*/
	UFUNCTION()
	void OnSlowZoneActorEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSlowZoneActorExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	virtual void Destroyed() override;
};
