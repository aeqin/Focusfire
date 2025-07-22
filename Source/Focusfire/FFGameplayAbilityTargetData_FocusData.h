#pragma once
#include "FFConstants_Struct.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "FFGameplayAbilityTargetData_FocusData.generated.h"

// This struct carries data to properly replicate FocusBase spawn across network
USTRUCT(BlueprintType)
struct FFGameplayAbilityTargetData_FocusData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
public:

	FFGameplayAbilityTargetData_FocusData()
	{ }

	// Spawn location & shoot direction of FocusBase
	UPROPERTY()
	FFStruct_FocusData FocusData;

	UPROPERTY()
	FPredictionKey FocusDataPredictionKey;

	// This is required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FFGameplayAbilityTargetData_FocusData::StaticStruct();
	}

	// This is required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		// The engine already defined NetSerialize for FName & FPredictionKey, thanks Epic!
		FocusData.NetSerialize(Ar, Map, bOutSuccess);
		FocusDataPredictionKey.NetSerialize(Ar, Map, bOutSuccess);
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FFGameplayAbilityTargetData_FocusData> : public TStructOpsTypeTraitsBase2<FFGameplayAbilityTargetData_FocusData>
{
	enum
	{
		WithNetSerializer = true // This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
