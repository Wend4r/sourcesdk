//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A list of physics bodies.
//
//=============================================================================//

#ifndef PHYSICS_BODYLIST_H
#define PHYSICS_BODYLIST_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

//-----------------------------------------------------------------------------
// A list of physics bodies (CEmptyPhysicsBodyList is the empty one).
//-----------------------------------------------------------------------------
abstract_class IPhysicsBodyList
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual ~IPhysicsBodyList() {}

	// Returns 0 in the empty list (a count-like query).
	virtual int Stub_3() = 0;

	// Returns null in the empty list.
	virtual void *Stub_4() = 0;

	// Returns null in the empty list.
	virtual void *Stub_5() = 0;
};

#endif // PHYSICS_BODYLIST_H
