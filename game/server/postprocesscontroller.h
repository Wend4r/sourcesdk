#ifndef POSTPROCESSCONTROLLER_H
#define POSTPROCESSCONTROLLER_H

#pragma once

#include "triggers.h"
#include "igameevents.h"

//-----------------------------------------------------------------------------
// CPostProcessingVolume adds no method to CBaseTrigger. The Linux table has 1 more slot, 280: the override of a method of IGameEventListener2, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseTriggerVTable<> >
abstract_class CPostProcessingVolumeVTable : public TBase, public IGameEventListener2
{
public:
	// Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
};

#endif // POSTPROCESSCONTROLLER_H
