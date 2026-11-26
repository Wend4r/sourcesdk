#ifndef PHYSOBJ_H
#define PHYSOBJ_H

#pragma once

#include "baseentity.h"
#include "../shared/imotionevent.h"

//-----------------------------------------------------------------------------
// CKeepUpright adds no method to CPointEntity (through CPointEntity, which adds no method). The Linux table has 1 more slot, 239: the override of a method of IMotionEvent, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CKeepUprightVTable : public TBase, public IMotionEvent
{
public:
	// Computes the angular force that keeps the object upright. Overrides IMotionEvent: a slot of this table on Linux only
	virtual void Simulate( void *pUnknown, IPhysicsBody *pBody, float flDeltaTime ) override = 0;
};

#endif // PHYSOBJ_H
