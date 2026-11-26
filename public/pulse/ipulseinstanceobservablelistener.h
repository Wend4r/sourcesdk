#ifndef IPULSEINSTANCEOBSERVABLELISTENER_H
#define IPULSEINSTANCEOBSERVABLELISTENER_H

#pragma once

#include "tier0/platform.h"

//-----------------------------------------------------------------------------
// Told when an observable value of a Pulse graph instance changes. A second base of
// CPulseGameBlackboard.
//
// Virtual methods only: one slot on both platforms, no destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseInstanceObservableListener
{
public:
	// CPulseGameBlackboard updates its networked blackboard here when pulse/client_blackboards is on
	virtual void Unk_0() = 0;
};

#endif // IPULSEINSTANCEOBSERVABLELISTENER_H
