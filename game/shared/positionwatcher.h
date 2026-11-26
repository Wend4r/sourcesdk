#ifndef POSITIONWATCHER_H
#define POSITIONWATCHER_H

#pragma once

#include "tier0/platform.h"

class CBaseEntity;
class IPhysicsBody;
class IPhysicsObject;

//-----------------------------------------------------------------------------
// Told when the entity it watches moves. A second base of CFuncNavBlocker,
// CFuncNavObstruction, CMarkupVolumeWithRef, CRopeKeyframe and CSplineConstraint.
//
// Virtual methods only, in the order of the game's table: the destructor (two slots
// on Linux, one on Windows) and one method.
//-----------------------------------------------------------------------------
abstract_class IPositionWatcher
{
public:
	virtual ~IPositionWatcher() {}

	// Called with the entity whose position changed
	virtual void NotifyPositionChanged( CBaseEntity *pEntity ) = 0;
};

//-----------------------------------------------------------------------------
// Told when a physics object it watches falls asleep or wakes up. A second base of
// CGenericConstraint, CPhysHinge and CPhysSlideConstraint.
//
// Virtual methods only, in the order of the game's table: the destructor (two slots
// on Linux, one on Windows) and two methods.
//-----------------------------------------------------------------------------
abstract_class IVPhysicsWatcher
{
public:
	virtual ~IVPhysicsWatcher() {}

	// Called when the physics object of the entity changes its sleep state
	virtual void NotifyVPhysicsStateChanged( IPhysicsObject *pPhysics, CBaseEntity *pEntity, bool bAwake ) = 0;
	// Called with a physics body and a flag; CPhysHinge applies m_hingeFriction to the body
	// when it is dynamic and the flag is set
	virtual void Unk_2( IPhysicsBody *pBody, bool bAwake ) = 0;
};

#endif // POSITIONWATCHER_H
