#ifndef PLAYER_PICKUP_H
#define PLAYER_PICKUP_H

#pragma once

#include "tier0/platform.h"

class CBasePlayerPawn;
class QAngle;

//-----------------------------------------------------------------------------
// The answers of an object a player carries. Three slots on both platforms, no destructor.
//-----------------------------------------------------------------------------
abstract_class IPlayerPickupVPhysics
{
public:
	// Whether the object has angles it prefers while the player carries it; CBreakableProp asks its prop data
	virtual bool HasPreferredCarryAnglesForPlayer( CBasePlayerPawn *pPlayer ) = 0;
	// The angles the object prefers while it is carried; CBreakableProp returns m_preferredCarryAngles
	virtual QAngle PreferredCarryAngles() = 0;
	// Empty by default, no class of the server overrides it
	virtual void Unk_2() = 0;
};

//-----------------------------------------------------------------------------
// The default answers of IPlayerPickupVPhysics (false, the zero angles, nothing). A second base
// of CBreakable, CBaseGrenade, CBreakableProp, CItem and CRagdollProp. No data: 8 bytes, the
// virtual table pointer; the members of the class below start right after it.
//-----------------------------------------------------------------------------
abstract_class CDefaultPlayerPickupVPhysics : public IPlayerPickupVPhysics
{
};

#endif // PLAYER_PICKUP_H
