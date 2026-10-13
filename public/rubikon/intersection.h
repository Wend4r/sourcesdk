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

// Flags of a pair of collision groups, see IPhysIntersectionController
typedef uint16 IntersectionLayerPairFlags;

#endif // RUBIKON_INTERSECTION_H
