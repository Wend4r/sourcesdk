//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Interfaces of the path finder.
//
//=============================================================================//

#ifndef NAV_PATHFIND_H
#define NAV_PATHFIND_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"

//-----------------------------------------------------------------------------
// A self test of the path finder (CPathfindGenericTest). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPathfindTest
{
public:
	// Named by what it does: "PathfindGeneric" in CPathfindGenericTest.
	virtual const char *GetName() const = 0;

	// Returns true in CPathfindGenericTest; CNavAreaBuildPathTest returns true when its first pointer is set and
	// its second is not.
	virtual bool Stub_1( const void *, const void * ) = 0;

	// Returns false in CPathfindGenericTest.
	virtual bool Stub_2() = 0;

	// Returns true in CPathfindGenericTest.
	virtual bool Stub_3() = 0;

	// Returns -1 in CPathfindGenericTest.
	virtual int Stub_4() = 0;

	// Named by what it does: runs the path query and compares it with the variant without arguments. Only the
	// slot is verified; the parameters are not declared.
	virtual bool RunTest() = 0;
};

//-----------------------------------------------------------------------------
// An obstacle of the path finder that may overlap others (SplitObs_t wraps one obstacle). No virtual
// destructor.
//-----------------------------------------------------------------------------
abstract_class IOverlapObs_t
{
public:
	// Returns a pointer member (SplitObs_t: the one of the wrapped obstacle).
	virtual void *Stub_0() const = 0;

	// A vector by value; SplitObs_t calls slot 4 of the wrapped obstacle and returns the scaled midpoint of the
	// two vectors it fills.
	virtual Vector Stub_1() = 0;

	// SplitObs_t calls slot 5 of the wrapped obstacle (four outputs) and clips the result by its float argument.
	virtual void Stub_2() = 0;
};

#endif // NAV_PATHFIND_H
