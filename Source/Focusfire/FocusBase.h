// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "FocusBase.generated.h"

// Forward Declarations
class USphereComponent;

UCLASS()
class FOCUSFIRE_API AFocusBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFocusBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Sphere collision */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> c_SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	FCollisionProfileName CollisionProfile = FName("Focus");

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/** GameplayAbilitySystem **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> c_AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return c_AbilitySystemComponent; }
};
