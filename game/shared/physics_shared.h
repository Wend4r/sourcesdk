//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared physics interfaces of the game.
//
//=============================================================================//

#ifndef PHYSICS_SHARED_H
#define PHYSICS_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"

//-----------------------------------------------------------------------------
// The contact of a collision event (CLegacyCollisionData).
//-----------------------------------------------------------------------------
abstract_class IPhysicsCollisionData
{
public:
	virtual Vector GetSurfaceNormal() = 0;
	virtual Vector GetContactPoint() = 0;
	virtual Vector GetContactSpeed() = 0;
};

#endif // PHYSICS_SHARED_H
