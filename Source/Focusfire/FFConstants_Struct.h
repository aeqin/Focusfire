#pragma once
#include "FFConstants_Enum.h"
#include "FFConstants_Struct.generated.h"

/*
 * This file contains a bunch of global constants used in Focusfire project (such as structs) 
 */

enum class EFocusType : uint8;

// This struct is the base struct used in Focusfire, contains a "null" boolean, for when struct is not filled with valid data
USTRUCT(BlueprintType)
struct FFStruct
{
	GENERATED_BODY()
public:
	// Constructor
	FFStruct()
	{
		bIsValid = false;
	}

	// Whether data of this FFStruct has been filled
	UPROPERTY()
	bool bIsValid = false;

	// This checks whether FFStruct child has been initialized with correct values, instead of being "null"
	virtual bool IsValidStruct() const
	{
		return bIsValid;
	}
	
	// For networking
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		// Value serialization
		Ar << bIsValid;
		
		bOutSuccess = true;
		return true;
	}

	// Destructor
	virtual ~FFStruct() = default;
};

template<>
struct TStructOpsTypeTraits<FFStruct> : public TStructOpsTypeTraitsBase2<FFStruct> // TODO check if I need to also do this template for inherited child classes
{
	enum
	{
		WithNetSerializer = true
	};
};

// This struct is the struct used in Focusfire that contains data on spawned FocusBases
USTRUCT(BlueprintType)
struct FFStruct_FocusData : public FFStruct
{
	GENERATED_BODY()
public:
	// Constructor
	FFStruct_FocusData(){}
	FFStruct_FocusData(const FVector& SpawnLocation, const FVector& SpawnDirection, const EFocusType& SpawnType)
	{
		this->SpawnLocation = SpawnLocation;
		this->SpawnDirection = SpawnDirection;
		this->FocusType = SpawnType;
		this->bIsValid = true;
	}

	// Spawn location of FocusBase
	UPROPERTY(BlueprintReadWrite)
	FVector SpawnLocation = FVector(0, 0, 0);

	// Shoot direction of FocusBase
	UPROPERTY(BlueprintReadWrite)
	FVector SpawnDirection = FVector(0, 0, 0); 

	// Type of FocusBase to shoot
	UPROPERTY(BlueprintReadWrite)
	EFocusType FocusType = EFocusType::INVALID;
	
	// For networking
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override
	{
		// Super
		Super::NetSerialize(Ar, Map, bOutSuccess);
		
		// Value serialization
		Ar << FocusType;

		// Struct serialization
		SpawnLocation.NetSerialize(Ar, Map, bOutSuccess);
		SpawnDirection.NetSerialize(Ar, Map, bOutSuccess);
		
		bOutSuccess = true;
		return true;
	}
};