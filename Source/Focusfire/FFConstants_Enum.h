#pragma once

/*
 * This file contains a bunch of global constants used in Focusfire project (such as enums) 
 */

UENUM(BlueprintType)
enum class EFocusType : uint8
{
	INVALID,
	FOCUS_REBOUND,
	FOCUS_METEOR,
	FOCUS_LAUNCH,
};
