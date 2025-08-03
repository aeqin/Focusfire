#pragma once

/*
 * This file contains a bunch of global constants used in Focusfire project (such as enums) 
 */

// Enum denotes the types of FocusBase
UENUM(BlueprintType)
enum class EFocusType : uint8
{
	INVALID,
	FOCUS_REBOUND,
	FOCUS_METEOR,
	FOCUS_LAUNCH,
};

// Enum denotes how to direct a FocusBase
UENUM(BlueprintType)
enum class EFocusDirective : uint8
{
	DEFAULT,            // Spawn at position
	SHOOT_IN_DIRECTION, // Spawn at position, then fire in direction
	SHOOT_TO_POSITION,  // Spawn at position, then move toward lock position
	TIME_DILATION,      // Control custom time dilation of the focus(inside focus slow zone)
};
