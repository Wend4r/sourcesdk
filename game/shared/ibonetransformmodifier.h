#ifndef IBONETRANSFORMMODIFIER_H
#define IBONETRANSFORMMODIFIER_H

#pragma once

#include "tier0/platform.h"

class CTransform;

//-----------------------------------------------------------------------------
// Lets an entity change bone transforms after the skeleton computed them. The
// skeleton instance keeps a pointer to it (handed over by CCSPlayerPawn::Activate
// and CCSWeaponBase::Activate); CCSPlayerPawn and CCSWeaponBase derive from it.
//
// Virtual methods only, in the order of the game's table (2 slots on both
// platforms, no destructor). The game has no symbols for it: both names say what
// the callers do with the methods.
//-----------------------------------------------------------------------------
abstract_class IBoneTransformModifier
{
public:
	// Called for every bone while the skeleton is set up, after the bone transform override,
	// with the parent bone, the bone and its 32-byte transform; true when it changed the
	// transform. CCSPlayerPawn scales the head by m_flBigHeadScale, CCSWeaponBase moves the
	// silencer and hand bones
	virtual bool ModifyBoneTransform( int nParentBone, int nBone, CTransform *pTransform ) = 0;
	// Called in a second loop over every bone of the skeleton with an index the model gives
	// for the bone and its transform; the result is not used. Returns false, never overridden on
	// the server or the client
	virtual bool ModifyBoneTransformSecondPass( int nBone, CTransform *pTransform ) = 0;
};

#endif // IBONETRANSFORMMODIFIER_H
