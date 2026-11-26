#ifndef PULSE_GAME_BLACKBOARD_H
#define PULSE_GAME_BLACKBOARD_H

#pragma once

#include "baseentity.h"
#include "pulse/ipulseinstanceobservablelistener.h"

//-----------------------------------------------------------------------------
// CPulseGameBlackboard adds no method to CBaseEntity. The Linux table has 1 more slot, 239: the override of a method of IPulseInstanceObservableListener, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CPulseGameBlackboardVTable : public TBase, public IPulseInstanceObservableListener
{
public:
	// Updates the networked blackboard when pulse/client_blackboards is on. Overrides IPulseInstanceObservableListener: a slot of this table on Linux only
	virtual void Unk_0() override = 0;
};

#endif // PULSE_GAME_BLACKBOARD_H
