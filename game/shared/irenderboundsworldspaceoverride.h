#ifndef IRENDERBOUNDSWORLDSPACEOVERRIDE_H
#define IRENDERBOUNDSWORLDSPACEOVERRIDE_H

#pragma once

#include "tier0/platform.h"
#include "mathlib/vector.h"

//-----------------------------------------------------------------------------
// Lets an entity give the world space render bounds of its scene node. The scene
// node keeps a pointer to it (CCSPlayerPawn::Spawn hands it over); CCSPlayerPawn
// derives from it.
//
// Virtual methods only (1 slot on both platforms, no destructor). The interface
// name is the one of the RTTI; the method has no symbol and is named by what it
// does.
//-----------------------------------------------------------------------------
abstract_class IRenderBoundsWorldSpaceOverride
{
public:
	// Asked first when the scene node computes its world space render bounds; true replaces the
	// bounds of the model and the physics. CCSPlayerPawn answers -80,-80,-64 .. 80,80,128 around
	// the origin while it is alive
	virtual bool GetRenderBoundsWorldSpace( Vector &vecMins, Vector &vecMaxs ) = 0;
};

#endif // IRENDERBOUNDSWORLDSPACEOVERRIDE_H
