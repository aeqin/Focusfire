// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent_ManagerFocus.h"

#include "BlueprintFunctionLib_FocusUtils.h"
#include "FFConstants_Struct.h"
#include "FocusBase.h"
#include "PingSphere.h"
#include "UserWidget_FocusMarker.h"
#include "Blueprint/UserWidget.h"

void UActorComponent_ManagerFocus::OnTickPositionOffscreenIndicators()
{
	APlayerController* _Player = GetWorld()->GetFirstPlayerController();

	// Iterate through all FocusBase in game
	for (TPair<AFocusBase*, UUserWidget_FocusMarker*>& Focus_Widget : Map_ClientFocus_Widget)
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

void UActorComponent_ManagerFocus::OnServerFocusDestroyed(AActor* DestroyedActor)
{
	if (AFocusBase* _focus = Cast<AFocusBase>(DestroyedActor))
	{
		if (Map_ClientFocus_Widget.Contains(_focus))
		{
			Map_ClientFocus_Widget[_focus]->RemoveFromParent(); // Remove widget (if it exists)
		}

		// Destroy the client version
		AFocusBase* _clientFocus = Map_ServerFocus_ClientFocus[_focus];
		_clientFocus->Destroy();

		// Remove FocusBase from refs
		Map_ClientFocus_Widget.Remove(_focus);
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

void UActorComponent_ManagerFocus::RegisterFocus(AFocusBase* NewFocus)
{
	// Server replicated FocusBase
	if(NewFocus->GetLocalRole() == ROLE_SimulatedProxy || NewFocus->GetLocalRole() == ROLE_AutonomousProxy)
	{
		// Check to see if there is already a Client-only FocusBase that needs to be synced
		AFocusBase* _focusToSync = nullptr;
		for (AFocusBase* _unsynced_focus : List_UnSynced_ClientFocus)
		{
			if (_unsynced_focus->GetFocusType() == NewFocus->GetFocusType())
			{
				_focusToSync = _unsynced_focus;
				break;
			}
		}

		// Found a Client-only focus that needs to be synced 
		if (_focusToSync)
		{
			_focusToSync->SetServerReplicatedFocusToFollow(NewFocus); // Set the Client-only FocusBase to follow the server-replicated leader
		}

		// NO Client-only focus, so create one
		else
		{
			_focusToSync = SpawnFocusOnClientOnly(NewFocus->GetOwner(), FFStruct_FocusData(NewFocus->GetActorLocation(), NewFocus->GetFocusType())); // Spawn new one
			_focusToSync->SetServerReplicatedFocusToFollow(NewFocus);
		}

		// Remove Client-only focus from un-synced list
		List_UnSynced_ClientFocus.RemoveSingle(_focusToSync);
		
		// Map Server replicated focus to Client-only focus, to destroy Client focus when Server focus is destroyed
		_focusToSync->ServerUID = NewFocus->ServerUID;
		Map_ServerFocus_ClientFocus.Add(NewFocus, _focusToSync);
		NewFocus->OnDestroyed.AddDynamic(this, &UActorComponent_ManagerFocus::OnServerFocusDestroyed); // When destroyed, remove refs

		UE_LOG(LogTemp, Warning, TEXT("DebugText ManagerFocus registered a new SERVER-replicated focus with ID %d"), _focusToSync->ServerUID);
	}

	// Client-only FocusBase
	else
	{
		Map_ClientFocus_Widget.Add(NewFocus, nullptr); // Store ref of this new FocusBase to map of widget

		UE_LOG(LogTemp, Warning, TEXT("DebugText ManagerFocus registered a new CLIENT-only focus"));
	}
}

AFocusBase* UActorComponent_ManagerFocus::SpawnFocusOnClientOnly(AActor* Spawner, const FFStruct_FocusData& FocusSpawnData)
{
	// Calculate transform
	FTransform _spawnTransform;
	_spawnTransform.SetLocation(FocusSpawnData.SpawnLocation);
	_spawnTransform.SetRotation(FQuat::Identity); // Set no rotation (so that widget always spawns on top)

	// Spawn FocusBase
	AFocusBase* _spawnedFocus = GetWorld()->SpawnActorDeferred<AFocusBase>(GetFocusClass(FocusSpawnData.FocusType), _spawnTransform, Spawner, Spawner->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (IsValid(_spawnedFocus))
	{
		_spawnedFocus->SetReplicates(false);
		_spawnedFocus->bIsClientLocalOnly = true;
		_spawnedFocus->FinishSpawning(_spawnTransform);
	}

	// After FocusBase is spawned, move it based on directive
	switch (FocusSpawnData.SpawnDirective)
	{
	case EFocusDirective::SHOOT_IN_DIRECTION:
		_spawnedFocus->ShootInDirection(FocusSpawnData.SpawnAfterVector);
		break;
	case EFocusDirective::SHOOT_TO_POSITION:
		_spawnedFocus->ShootToLocation(FocusSpawnData.SpawnAfterVector);
		break;
	default:
		break;
	}

	// Add to list of un-synced focuses
	List_UnSynced_ClientFocus.Add(_spawnedFocus);
	
	return _spawnedFocus;
}

AFocusBase* UActorComponent_ManagerFocus::SpawnFocusOnClientOnly_Then_SpawnOnServer_RPC(AActor* Spawner, const FFStruct_FocusData& FocusSpawnData)
{
	// Create Client-only focus
	AFocusBase* _spawnedFocus = SpawnFocusOnClientOnly(Spawner, FocusSpawnData);

	// Request Server to create replicated focus
	RPC_Server_SpawnFocus(Spawner, FocusSpawnData);
	
	return _spawnedFocus;
}

void UActorComponent_ManagerFocus::RPC_Server_SpawnFocus_Implementation(AActor* Spawner, const FFStruct_FocusData& FocusSpawnData)
{
	// Calculate transform
	FTransform _spawnTransform;
	_spawnTransform.SetLocation(FocusSpawnData.SpawnLocation);
	_spawnTransform.SetRotation(FQuat::Identity); // Set no rotation (so that widget always spawns on top)

	// Spawn FocusBase
	AFocusBase* _spawnedFocus = GetWorld()->SpawnActorDeferred<AFocusBase>(GetFocusClass(FocusSpawnData.FocusType), _spawnTransform, Spawner, Spawner->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (IsValid(_spawnedFocus))
	{
		_spawnedFocus->SetReplicates(true);
		_spawnedFocus->bIsClientLocalOnly = false; // This is server-replicated FocusBase, so set to false
		_spawnedFocus->ServerUID = Generator_CurrentValid_UID++;
		_spawnedFocus->FinishSpawning(_spawnTransform);
	}

	// After FocusBase is spawned, move it based on directive
	switch (FocusSpawnData.SpawnDirective)
	{
	case EFocusDirective::SHOOT_IN_DIRECTION:
		_spawnedFocus->ShootInDirection(FocusSpawnData.SpawnAfterVector);
		RPC_Client_DirectClientPredictedFocus(_spawnedFocus, FocusSpawnData.SpawnDirective);
		break;
	case EFocusDirective::SHOOT_TO_POSITION:
		_spawnedFocus->ShootToLocation(FocusSpawnData.SpawnAfterVector);
		break;
	default:
		break;
	}

	UE_LOG(LogTemp, Warning, TEXT("DebugText ManagerFocus Server spawned a server-replicated focus"));
	
	// TODO add to list of all server focuses? #ccc 
}

void UActorComponent_ManagerFocus::RPC_Client_DirectClientPredictedFocus_Implementation(
	AFocusBase* ServerReplicatedFocus, const EFocusDirective& Directive, const FVector& DirectiveVector, const float& DirectiveFloat)
{
	if (Map_ServerFocus_ClientFocus.Contains(ServerReplicatedFocus))
	{
		// Direct client-only FocusBase here
	}
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

FLinearColor UActorComponent_ManagerFocus::GetFocusColor(EFocusType FocusType)
{
	if (Map_FocusType_Color.Contains(FocusType))
	{
		return Map_FocusType_Color[FocusType];
	}
	else
	{
		return FLinearColor::White;
	}
}

FString UActorComponent_ManagerFocus::GetFocusString(EFocusType FocusType)
{
	if (Map_FocusType_String.Contains(FocusType))
	{
		return Map_FocusType_String[FocusType];
	}
	else
	{
		return FString::Printf(TEXT("Error: There is no String name mapped to this EFocusType [%s]"), *UEnum::GetValueAsString(FocusType));
	}
}

TSubclassOf<AFocusBase> UActorComponent_ManagerFocus::GetFocusClass(EFocusType FocusType)
{
	if (Map_FocusType_FocusClass.Contains(FocusType))
	{
		return Map_FocusType_FocusClass[FocusType];
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ManagerFocus ERROR: Attempted to get a focus class for [%s], which isn't mapped"), *UEnum::GetValueAsString(FocusType));
		return AFocusBase::StaticClass();
	}
}
