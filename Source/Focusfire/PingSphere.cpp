// Fill out your copyright notice in the Description page of Project Settings.


#include "PingSphere.h"

#include "UserWidget_FocusMarker.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
APingSphere::APingSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create sphere collider
	c_SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(c_SphereComponent);
	c_SphereComponent->SetSphereRadius(52.0f);
	c_SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // At beginning, PingSphere is prospective, and should not be raycast-able
	c_SphereComponent->SetHiddenInGame(false); // See (in-game) debug collision sphere

	// Create static mesh
	c_StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	c_StaticMeshComponent->AttachToComponent(c_SphereComponent, FAttachmentTransformRules::KeepRelativeTransform);
	c_StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create widget component
	FocusMarkerWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FocusMarkerComponent"));
	FocusMarkerWidgetComponent->SetRelativeLocation(FVector(0, 0, c_SphereComponent->GetScaledSphereRadius() + 30.0f));
	FocusMarkerWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	FocusMarkerWidgetComponent->AttachToComponent(c_SphereComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void APingSphere::BeginPlay()
{
	Super::BeginPlay();

	// Set default texture
	if (mat_ProspectivePing && c_StaticMeshComponent)
	{
		c_StaticMeshComponent->SetMaterial(0, mat_ProspectivePing);
	}

	// Set actor location in the widget display distance from player
	if (UUserWidget_FocusMarker* _FocusMarkerWidget = Cast<UUserWidget_FocusMarker>(FocusMarkerWidgetComponent->GetUserWidgetObject()))
	{
		FocusMarkerWidget = _FocusMarkerWidget; // Store ref
		FocusMarkerWidget->ActorToTrack = this;
	}
}

void APingSphere::TickLifetimeTimer()
{
	LifeInSecondsCurr -= 1; // Timer ticks down by 1 every second
	if (LifeInSecondsCurr < 1)
	{
		GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle); // Clear timer
		Destroy(); // Destroy this PingSphere
	}
}

// Called every frame
void APingSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APingSphere::SetPingActive()
{
	// Set active texture
	if (mat_ActivePing && c_StaticMeshComponent)
	{
		c_StaticMeshComponent->SetMaterial(0, mat_ActivePing);
	}

	// Create a timer that ticks down, when over, destroys this PingSphere
	LifeInSecondsCurr = LifeInSecondsMax;
	GetWorld()->GetTimerManager().SetTimer(LifeTimerHandle, this, &APingSphere::TickLifetimeTimer,
										   1.0f, true);

	// Change the collision so that Player may find it with a raycast
	c_SphereComponent->SetCollisionProfileName(CollisionProfile.Name, true);
	c_SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // Raycast-able, but won't physically block player
}

