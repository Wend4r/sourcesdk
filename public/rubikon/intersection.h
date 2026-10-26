//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Rubikon collision group and interaction layer indices
//
//===========================================================================//

#ifndef RUBIKON_INTERSECTION_H
#define RUBIKON_INTERSECTION_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

// Index of a registered collision group
typedef int32 CollisionGroupIndex;

// Index of a registered interaction layer, the bit position in the interaction masks
typedef int32 IntersectionLayerIndex;

// Flags of a pair of collision groups, see IPhysIntersectionController; a contact keeps the flags of its pair
enum IntersectionPairFlags_t
{
	INTERSECTION_PAIR_RESOLVE_CONTACTS = ( 1 << 0 ),
	INTERSECTION_PAIR_NOTIFY_TOUCH_FOUND = ( 1 << 2 ),
	INTERSECTION_PAIR_NOTIFY_TOUCH_PERSISTS = ( 1 << 3 ),
	INTERSECTION_PAIR_NOTIFY_TOUCH_LOST = ( 1 << 4 ),

	INTERSECTION_PAIR_TRIGGER = INTERSECTION_PAIR_NOTIFY_TOUCH_FOUND | INTERSECTION_PAIR_NOTIFY_TOUCH_LOST,
	INTERSECTION_PAIR_DEFAULT_COLLISION = INTERSECTION_PAIR_RESOLVE_CONTACTS,
	INTERSECTION_NOTIFICATION_ANY = INTERSECTION_PAIR_NOTIFY_TOUCH_FOUND | INTERSECTION_PAIR_NOTIFY_TOUCH_PERSISTS | INTERSECTION_PAIR_NOTIFY_TOUCH_LOST,
};

// A set of IntersectionPairFlags_t bits
typedef uint16 IntersectionLayerPairFlags;

#endif // RUBIKON_INTERSECTION_H
