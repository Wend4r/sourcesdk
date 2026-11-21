//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Notifications a physics world sends to the game.
//
//=============================================================================//

#ifndef IPHYSICSWORLDEVENTLISTENER_H
#define IPHYSICSWORLDEVENTLISTENER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class IVPhysics2World;

//-----------------------------------------------------------------------------
// One slot on both platforms, no destructor.
//-----------------------------------------------------------------------------
abstract_class IPhysicsWorldEventListener
{
public:
	virtual void OnWorldRequiresDebugRendering( IVPhysics2World *pWorld, bool bRequiresDebugRendering ) = 0;
};

#endif // IPHYSICSWORLDEVENTLISTENER_H
