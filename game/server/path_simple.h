#ifndef PATH_SIMPLE_H
#define PATH_SIMPLE_H

#pragma once

#include "baseentity.h"
#include "../shared/iphysicsspline.h"

//-----------------------------------------------------------------------------
// CPathSimple adds 2 methods to CBaseEntity, Windows slots 238-239.
// The Linux table has 7, slots 239-245: 5 override methods of IPhysicsSpline, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CPathSimpleVTable : public TBase, public IPhysicsSpline
{
public:
	// Clears the spline at +2096 and parses the path string (+2176) into nodes. Named by what it does
	virtual void BuildPath() = 0;
	// Node count of the spline, building it first when empty. Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual int GetPointCount() override = 0;
	// Node positions in world space. Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual void GetWorldPositions( CUtlVector< Vector > *pOut ) override = 0;
	// In-tangents in world space. Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual void GetWorldInTangents( CUtlVector< Vector > *pOut ) override = 0;
	// Out-tangents in world space. Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual void GetWorldOutTangents( CUtlVector< Vector > *pOut ) override = 0;
	// Evaluates the spline (a large function). Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual void Unk_5( int nUnknown, void *pUnknown1, void *pUnknown2, int *pUnknown3 ) override = 0;
	// Empty in every class; called four times by the path mover code
	virtual void Stub_239() = 0;
};

//-----------------------------------------------------------------------------
// CPathWithDynamicNodes adds 2 methods to CPathSimple: Linux slots 246-247, Windows 240-241.
//-----------------------------------------------------------------------------
template < class TBase = CPathSimpleVTable<> >
abstract_class CPathWithDynamicNodesVTable : public TBase
{
public:
	// Returns "path_node". Named by what it does
	virtual const char *GetPathNodeClassName() = 0;
	// Rebuilds the spline from m_vecPathNodes. Named by what it does
	virtual void UpdatePathFromNodes() = 0;
};

#endif // PATH_SIMPLE_H
