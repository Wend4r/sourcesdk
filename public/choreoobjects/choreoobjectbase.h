//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The base of the objects of a choreographed scene (actors, channels, events and the
//          scene itself).
//
//=============================================================================//

#ifndef CHOREOOBJECTBASE_H
#define CHOREOOBJECTBASE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"

class IBaseToolAttr;

//-----------------------------------------------------------------------------
// The base of every choreo object: the tool attribute description of the class and an id.
// No virtual destructor: CChoreoEvent and CChoreoScene introduce their own, actors and channels
// are destroyed by their scene without one.
//-----------------------------------------------------------------------------
abstract_class CChoreoObjectBase
{
public:
	// Named by what it does: a static object per class that describes the class to the tool
	// attribute system; its type is not reconstructed
	virtual const void *GetClassAttributeInfo() const = 0;
	// Appends the tool attributes of every level of the class, the base first
	virtual void GetAttributeTable( CUtlVector< IBaseToolAttr * > *pAttributes ) = 0;

public:
	// Named by what it holds: the next value of a counter shared by every choreo object, taken by
	// the constructor; CChoreoEvent::CopyFrom copies it
	int m_nGlobalID;
};

#endif // CHOREOOBJECTBASE_H
