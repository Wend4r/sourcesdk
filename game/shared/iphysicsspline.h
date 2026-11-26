#ifndef IPHYSICSSPLINE_H
#define IPHYSICSSPLINE_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "mathlib/vector.h"

//-----------------------------------------------------------------------------
// A spline the physics can follow. A second base of CPathSimple and
// CPathParticleRope.
//
// Virtual methods only, in the order of the game's table: the destructor (two slots
// on Linux, one on Windows) and five methods. The names are given by what CPathSimple
// does with them.
//-----------------------------------------------------------------------------
abstract_class IPhysicsSpline
{
public:
	virtual ~IPhysicsSpline() {}

	// The number of nodes of the spline; CPathSimple builds the spline first when it is empty
	virtual int GetPointCount() = 0;
	// The node positions in world space
	virtual void GetWorldPositions( CUtlVector< Vector > *pOut ) = 0;
	// The in-tangents of the nodes in world space
	virtual void GetWorldInTangents( CUtlVector< Vector > *pOut ) = 0;
	// The out-tangents of the nodes in world space
	virtual void GetWorldOutTangents( CUtlVector< Vector > *pOut ) = 0;
	// Empty by default; CPathSimple evaluates the spline here (a large function)
	virtual void Unk_5( int nUnknown, void *pUnknown1, void *pUnknown2, int *pUnknown3 ) = 0;
};

#endif // IPHYSICSSPLINE_H
