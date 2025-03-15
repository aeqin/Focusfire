// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_ManagerFocus.h"

#include "BlueprintFunctionLib_FocusUtils.h"
#include "FocusBase.h"
#include "PingSphere.h"
#include "UserWidget_FocusMarker.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

void UActorComponent_ManagerFocus::OnTickPositionOffscreenIndicators()
{
	APlayerController* _Player = GetWorld()->GetFirstPlayerController();

	// Iterate through all FocusBase in game
	for (TPair<AFocusBase*, UUserWidget_FocusMarker*>& Focus_Widget : Map_Focus_Widget)
	{
		AFocusBase* _Focus = Focus_Widget.Key;
		UUserWidget_FocusMarker* _Widget = Focus_Widget.Value;

		// FocusBase's WidgetComponent is VISIBLE on the Player's viewport
		if (UBlueprintFunctionLib_FocusUtils::IsLocationOnPlayerScreen(_Player, _Focus->GetActorLocation()))
		{
			if (IsValid(_Widget))
			{
				_Widget->RemoveFromParent(); // Remove the offscreen indicator widget
			}
		}

		// FocusBase's WidgetComponent is OFF the Player's viewport
		else
		{
			// If no widget exists for this FocusBase
			if (not IsValid(_Widget))
			{
				// Create a widget (for offscreen indicator)
				Focus_Widget.Value = CreateWidget<UUserWidget_FocusMarker>(GetWorld(), FocusMarkerWidgetClass);
				Focus_Widget.Value->ActorToTrack = _Focus;
				Focus_Widget.Value->SetFocusMarkerMode(EFocusMarkerMode::OFF_SCREEN); // Set mode so that image is offscreen indicator
			}
			// Widget already exists
			else
			{
				if (not _Widget->IsInViewport())
				{
					_Widget->AddToViewport(); // Add the offscreen indicator
				}

				// Reposition & rotate the offscreen indicator
				PositionOffscreenIndicators(_Player, _Focus->GetWidgetComponent(), _Widget);
			}
		}
	}

	// Iterate through all PingSphere in game
	for (TPair<APingSphere*, UUserWidget_FocusMarker*>& Ping_Widget : Map_Ping_Widget)
	{
		APingSphere* _Ping = Ping_Widget.Key;
		UUserWidget_FocusMarker* _Widget = Ping_Widget.Value;

		// PingSphere's WidgetComponent is VISIBLE on the Player's viewport
		if (UBlueprintFunctionLib_FocusUtils::IsLocationOnPlayerScreen(_Player, _Ping->GetActorLocation()))
		{
			if (IsValid(_Widget))
			{
				_Widget->RemoveFromParent(); // Remove the offscreen indicator widget
			}
		}

		// PingSphere's WidgetComponent is OFF the Player's viewport
		else
		{
			// If no widget exists for this PingSphere
			if (not IsValid(_Widget))
			{
				// Create a widget (for offscreen indicator)
				Ping_Widget.Value = CreateWidget<UUserWidget_FocusMarker>(GetWorld(), FocusMarkerWidgetClass);
				Ping_Widget.Value->ActorToTrack = _Ping;
				Ping_Widget.Value->SetFocusMarkerMode(EFocusMarkerMode::OFF_SCREEN); // Set mode so that image is offscreen indicator
			}
			// Widget already exists
			else
			{
				if (not _Widget->IsInViewport())
				{
					_Widget->AddToViewport(); // Add the offscreen indicator
				}

				// Reposition & rotate the offscreen indicator
				PositionOffscreenIndicators(_Player, _Ping->GetWidgetComponent(), _Widget);
			}
		}
	}
}

void UActorComponent_ManagerFocus::PositionOffscreenIndicators(const APlayerController* PlayerController, const UWidgetComponent* WidgetComponent, UUserWidget_FocusMarker* MarkerWidget)
{
	float _Angle =  UBlueprintFunctionLib_FocusUtils::GetPlayerToWidgetComponentAngle(PlayerController, WidgetComponent);
	FVector2D _WidgetWithinViewportOffset = MarkerWidget->WithinViewportOffset;
	int _ViewportSizeX;
	int _ViewportSizeY;
	PlayerController->GetViewportSize(_ViewportSizeX,_ViewportSizeY); // Get size of Player screen

	// Subtract size of widget (so widget doesn't go off the screen bottom and right)
	_ViewportSizeX -= _WidgetWithinViewportOffset.X / 2;
	_ViewportSizeY -= _WidgetWithinViewportOffset.Y / 2;
	
	// Reposition the offscreen indicator (around the border of the screen)
	FVector2D _Position = UBlueprintFunctionLib_FocusUtils::GetViewportBoundsIntersectionFromAngle(FVector2D(_ViewportSizeX, _ViewportSizeY), _Angle);
	MarkerWidget->SetPositionInViewport(_Position);

	// Rotate the offscreen indicator (to point from Player to FocusBase)
	MarkerWidget->OnTickRotateIndicatorImage(_Angle);
}

void UActorComponent_ManagerFocus::OnFocusDestroyed(AActor* DestroyedActor)
{
	if (AFocusBase* _focus = Cast<AFocusBase>(DestroyedActor))
	{
		if (UUserWidget_FocusMarker* _widget = Map_Focus_Widget[_focus])
		{
			Map_Focus_Widget[_focus]->RemoveFromParent(); // Remove widget (if it exists)
		}
		Map_Focus_Widget.Remove(_focus); // Remove FocusBase from ref
	}
}

void UActorComponent_ManagerFocus::OnPingDestroyed(AActor* DestroyedActor)
{
	if (APingSphere* _ping = Cast<APingSphere>(DestroyedActor))
	{
		if (UUserWidget_FocusMarker* _widget = Map_Ping_Widget[_ping])
		{
			Map_Ping_Widget[_ping]->RemoveFromParent(); // Remove widget (if it exists)
		}
		Map_Ping_Widget.Remove(_ping); // Remove PingSphere from ref
	}
}

// Sets default values for this component's properties
UActorComponent_ManagerFocus::UActorComponent_ManagerFocus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UActorComponent_ManagerFocus::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UActorComponent_ManagerFocus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Position offscreen indicators for FocusBase
	OnTickPositionOffscreenIndicators();
}

AFocusBase* UActorComponent_ManagerFocus::ShootFocusInDirection(FTransform SpawnTransform, FVector ShootDirection,
	TSubclassOf<class AFocusBase> FocusTypeToSpawn, AActor* Spawner)
{
	SpawnTransform.SetRotation(FQuat::Identity); // Set no rotation (so that widget always spawns on top)
	AFocusBase* _spawnedFocus = GetWorld()->SpawnActorDeferred<AFocusBase>(FocusTypeToSpawn, SpawnTransform, Spawner->GetOwner(), Spawner->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (IsValid(_spawnedFocus))
	{
		_spawnedFocus->FinishSpawning(SpawnTransform);
		_spawnedFocus->ShootInDirection(ShootDirection);
		
		Map_Focus_Widget.Add(_spawnedFocus, nullptr); // Store ref if this new FocusBase to map
		_spawnedFocus->OnDestroyed.AddDynamic(this, &UActorComponent_ManagerFocus::OnFocusDestroyed); // When destroyed, remove ref from map
	}
	return _spawnedFocus;
}

APingSphere* UActorComponent_ManagerFocus::SpawnPing(FTransform SpawnTransform, TSubclassOf<class APingSphere> PingClassToSpawn,
	AActor* Spawner)
{
	SpawnTransform.SetRotation(FQuat::Identity); // Set no rotation (so that widget always spawns on top)
	APingSphere* _spawnedPing = GetWorld()->SpawnActorDeferred<APingSphere>(PingClassToSpawn, SpawnTransform, Spawner->GetOwner(), Spawner->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (IsValid(_spawnedPing))
	{
		_spawnedPing->FinishSpawning(SpawnTransform);
		
		Map_Ping_Widget.Add(_spawnedPing, nullptr); // Store ref if this new PingSphere to map
		_spawnedPing->OnDestroyed.AddDynamic(this, &UActorComponent_ManagerFocus::OnPingDestroyed); // When destroyed, remove ref from map
	}
	return _spawnedPing;
}

FLinearColor UActorComponent_ManagerFocus::GetFocusColor(TSubclassOf<AFocusBase> FocusType)
{
	if (Map_Focusclass_Color.Contains(FocusType))
	{
		return Map_Focusclass_Color[FocusType];
	}
	else
	{
		return FLinearColor::White;
	}
}

FString UActorComponent_ManagerFocus::GetFocusString(TSubclassOf<AFocusBase> FocusType)
{
	if (Map_Focusclass_String.Contains(FocusType))
	{
		return Map_Focusclass_String[FocusType];
	}
	else
	{
		return FString::Printf(TEXT("Error: There is no String name mapped to this FocusBase class [%s]"), *FocusType->GetName());
	}
}
