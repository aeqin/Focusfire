// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusBase.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

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

void AFocusBase::LockInPlace()
{
	c_ProjectileMovementComponent->Velocity = FVector(0, 0, 0);
}
