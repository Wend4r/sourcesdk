#ifndef ROPE_H
#define ROPE_H

#pragma once

#include "../shared/basemodelentity.h"
#include "../shared/positionwatcher.h"

//-----------------------------------------------------------------------------
// CRopeKeyframe adds 1 method to CBaseModelEntity, Windows slot 271.
// The Linux table has 2, slots 272-273: 1 override methods of IPositionWatcher, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CRopeKeyframeVTable : public TBase, public IPositionWatcher
{
public:
	// Updates the rope when an end point entity moves. Overrides IPositionWatcher: a slot of this table on Linux only
	virtual void NotifyPositionChanged( CBaseEntity *pEntity ) override = 0;
	virtual void Init() = 0;
};

#endif // ROPE_H
