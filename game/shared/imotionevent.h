#ifndef IMOTIONEVENT_H
#define IMOTIONEVENT_H

#pragma once

#include "tier0/platform.h"

class IPhysicsBody;

//-----------------------------------------------------------------------------
// The callback of a physics motion controller. A second base of CKeepUpright and
// CTriggerPhysics; CBuoyancyHelper, CConstantForceController, CPhysicsShake and the ragdoll
// conflict fixup of CBaseAnimGraph implement it as well.
//
// Virtual methods only, in the order of the game's table (6 slots on both platforms,
// no destructor). Only Simulate has a name: the one large override of the table, which
// applies the forces of the controller to a body.
//-----------------------------------------------------------------------------
abstract_class IMotionEvent
{
public:
	// Empty by default; CTriggerPhysics updates its controller here unless m_bDisabled
	virtual void Unk_0() = 0;
	// Empty by default; the ragdoll conflict fixup overrides it
	virtual void Unk_1() = 0;
	// Applies the forces of the controller to the body for one step. The first argument is
	// not read by any implementation
	virtual void Simulate( void *pUnknown, IPhysicsBody *pBody, float flDeltaTime ) = 0;
	// Empty by default; CBuoyancyHelper updates the buoyancy of each body here
	virtual void Unk_3() = 0;
	// Empty in every implementation
	virtual void Unk_4() = 0;
	// Returns false by default; CPhysicsShake and the ragdoll conflict fixup return true
	virtual bool Unk_5() = 0;
};

#endif // IMOTIONEVENT_H
