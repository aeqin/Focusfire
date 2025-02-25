// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusBase.h"
#include "FocusBase.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AFocusBase::AFocusBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create sphere collider
	c_SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(c_SphereComponent);
	c_SphereComponent->SetCollisionProfileName(CollisionProfile.Name, true);
	c_SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Raycast-able, but won't physically block player
	c_SphereComponent->SetHiddenInGame(false); // See (in-game) debug collision sphere

	// Create VFX to represent FocusBase in game
	c_NiagaraComponent_RepresentingVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	
	// Create projectile motion component
	c_ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	c_ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	
	// Create and initialize the AbilitySystemComponent
	c_AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

// Called when the game starts or when spawned
void AFocusBase::BeginPlay()
{
	Super::BeginPlay();

	// Create a timer that ticks down, when over, destroys this FocusBase
	LifeInSecondsCurr = LifeInSecondsMax;
	GetWorld()->GetTimerManager().SetTimer(LifeTimerHandle, this, &AFocusBase::TickLifetimeTimer,
										   1.0f, true);

	// Add in NiagaraSystem for VFX representing FocusBase in-game
	if (NiagaraSystem_RepresentingVFX)
	{
		c_NiagaraComponent_RepresentingVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem_RepresentingVFX, c_SphereComponent, NAME_None, FVector(0.0f), FRotator(0.f), EAttachLocation::Type::SnapToTargetIncludingScale, true);
	}
}

void AFocusBase::TickLifetimeTimer()
{
	LifeInSecondsCurr -= 1; // Timer ticks down by 1 every second
	if (LifeInSecondsCurr < 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle); // Clear timer
		Destroy(); // Destroy this FocusBase
	}
}

void AFocusBase::ActivateAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("ccc Activating %s Ability"), *GetName());
	// Override in children
}

// Called every frame
void AFocusBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFocusBase::ShootInDirection(const FVector Direction)
{
	c_ProjectileMovementComponent->Velocity = Direction.GetSafeNormal() * ShootSpeed;
}

void AFocusBase::LockInPlace(bool bResetLifetime)
{
	c_ProjectileMovementComponent->Velocity = FVector(0, 0, 0);
	c_ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	// If flag to reset lifetime is set, then reset lifetime of FocusBase
	if (bResetLifetime)
	{
		LifeInSecondsCurr = LifeInSecondsMax;
	}
}
