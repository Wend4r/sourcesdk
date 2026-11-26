//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Recorded tracks of values.
//
//=============================================================================//

#ifndef DEMO_TRACKS_H
#define DEMO_TRACKS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"

//-----------------------------------------------------------------------------
// A recorded track of values (float, Vec3D< float > and Quaternion tracks), 32 bytes. The game only destroys
// these tracks (the vector is purged, its elements have no destructor); no code adds to one, so the element type
// of the vector is not shown by the game. The server library of Windows has no table of its own for the
// abstract template.
//-----------------------------------------------------------------------------
template < class T >
abstract_class DemoTrackBase_t
{
public:
	// Pure virtual; no implementation in the server.
	virtual void Stub_0() = 0;

public:
	// Named by what the code does with it: the recorded values. The element type is the template argument by
	// the class name only; no code reads or writes an element
	CUtlVector< T > m_Values;
};

#endif // DEMO_TRACKS_H
