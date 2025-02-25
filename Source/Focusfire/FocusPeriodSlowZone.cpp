// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusPeriodSlowZone.h"

#include "FocusBase.h"
#include "FocusfireCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
AFocusPeriodSlowZone::AFocusPeriodSlowZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Set up area of effect
	c_SphereOfSlowEffect = CreateDefaultSubobject<USphereComponent>(TEXT("SphereOfSlowEffectComponent"));
	SetRootComponent(c_SphereOfSlowEffect); // Set collision shape as root component
	c_SphereOfSlowEffect->SetSphereRadius(RadiusOfSlowEffect);
	c_SphereOfSlowEffect->SetCollisionProfileName(CollisionProfile.Name, true);
	c_SphereOfSlowEffect->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	c_SphereOfSlowEffect->SetHiddenInGame(false); // Set visible in game for debugging
}

void AFocusPeriodSlowZone::SetActorSlowdownException(AActor* ActorToIgnore, bool bDoAddException)
{
	if (bDoAddException)
	{
		Set_IgnoreSlowdownActors.Add(ActorToIgnore);

		// Being ignored, so make sure time dilation is set back to default
		ActorToIgnore->CustomTimeDilation = 1.0;
		Set_SlowdownActors.Remove(ActorToIgnore);
	}
	else
	{
		Set_IgnoreSlowdownActors.Remove(ActorToIgnore);

		// No longer being ignore, so slow down
		ActorToIgnore->CustomTimeDilation = StrengthOfTimeSlowdown;
		Set_SlowdownActors.Add(ActorToIgnore);
	}
}

// Called when the game starts or when spawned
void AFocusPeriodSlowZone::BeginPlay()
{
	Super::BeginPlay();

	if (c_SphereOfSlowEffect)
	{
		c_SphereOfSlowEffect->OnComponentBeginOverlap.AddDynamic(this, &AFocusPeriodSlowZone::OnSlowZoneActorEntered);
		c_SphereOfSlowEffect->OnComponentEndOverlap.AddDynamic(this, &AFocusPeriodSlowZone::OnSlowZoneActorExited);
	}
}

void AFocusPeriodSlowZone::OnSlowZoneActorEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* _slowDownActor = nullptr;
	
	// Check if overlapped actor is a FocusBase
	if (AFocusBase* _focusBase = Cast<AFocusBase>(OtherActor))
	{
		_slowDownActor = _focusBase;
		UE_LOG(LogTemp, Warning, TEXT("ccc Overlapped by AFocusBase"));
	}
	// Check if overlapped actor is a FocusfireCharacter
	else if (AFocusfireCharacter* _focusfireCharacter = Cast<AFocusfireCharacter>(OtherActor))
	{
		_slowDownActor = _focusfireCharacter;
		UE_LOG(LogTemp, Warning, TEXT("ccc Overlapped by AFocusfireCharacter"));
	}

	// If AActor is one that can be slowed down by FocusPeriodZone, then add it to list
	if (_slowDownActor)
	{
		_slowDownActor->CustomTimeDilation = StrengthOfTimeSlowdown;
		Set_SlowdownActors.Add(_slowDownActor);
	}
}

void AFocusPeriodSlowZone::OnSlowZoneActorExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If AActor was slowed down by FocusPeriodZone, then returns its time dilation back to normal
	if (Set_SlowdownActors.Contains(OtherActor))
	{
		OtherActor->CustomTimeDilation = 1.0;
		Set_SlowdownActors.Remove(OtherActor);
	}
}

void AFocusPeriodSlowZone::Destroyed()
{
	// Returns every AActor slowed down by FocusPeriodSlowZone back to normal time dilation
	for (AActor* _slowedActor : Set_SlowdownActors)
	{
		if (IsValid(_slowedActor))
			_slowedActor->CustomTimeDilation = 1.0;
	}
	
	Super::Destroyed();
}

