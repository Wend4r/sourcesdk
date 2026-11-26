#ifndef PHYSFORCE_H
#define PHYSFORCE_H

#pragma once

#include "baseentity.h"

class IPhysicsObject;

typedef Vector AngularImpulse;

//-----------------------------------------------------------------------------
// CPhysForce adds 2 methods to CPointEntity (through CPointEntity, which adds no method): Linux slots 239-240, Windows 238-239.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CPhysForceVTable : public TBase
{
public:
	// The force and torque of one step for the physics object; CPhysThruster and CPhysTorque override it
	virtual void SetupForces( IPhysicsObject *pPhys, Vector &linear, AngularImpulse &angular ) = 0;
	// Called when the force is switched on; CPhysThruster and CPhysTorque override it
	virtual void OnActivate() = 0;
};

#endif // PHYSFORCE_H
