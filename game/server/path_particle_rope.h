#ifndef PATH_PARTICLE_ROPE_H
#define PATH_PARTICLE_ROPE_H

#pragma once

#include "baseentity.h"
#include "../shared/iphysicsspline.h"

//-----------------------------------------------------------------------------
// CPathParticleRope adds no method to CBaseEntity. The Linux table has 4 more slots, 239-242: overrides of methods of IPhysicsSpline, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CPathParticleRopeVTable : public TBase, public IPhysicsSpline
{
public:
	// Empty. Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual int GetPointCount() override = 0;
	// Compares a networked vector of the rope with the argument. Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual void GetWorldPositions( CUtlVector< Vector > *pOut ) override = 0;
	// Compares a second networked vector of the rope with the argument. Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual void GetWorldInTangents( CUtlVector< Vector > *pOut ) override = 0;
	// Compares a third networked vector of the rope with the argument. Overrides IPhysicsSpline: a slot of this table on Linux only
	virtual void GetWorldOutTangents( CUtlVector< Vector > *pOut ) override = 0;
};

#endif // PATH_PARTICLE_ROPE_H
