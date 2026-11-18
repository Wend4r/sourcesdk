#ifndef CSTRIKE15_CS_PLAYER_MOVEMENTSERVICES_H
#define CSTRIKE15_CS_PLAYER_MOVEMENTSERVICES_H

#pragma once

#include "../player_movementservices.h"

//-----------------------------------------------------------------------------
// CCSPlayer_MovementServices, slot 58. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_MovementServices_HumanoidVTable<> >
abstract_class CCSPlayer_MovementServicesVTable : public TBase
{
public:
	// Plays the step sound like PlayStepSound, then counts the step in m_iFootsteps and
	// fires the player_footstep event
	virtual void PlayFootstep( const Vector &vecOrigin, const CPhysSurfaceProperties *pSurface, float flVolume, bool bForce ) = 0;
};

#endif // CSTRIKE15_CS_PLAYER_MOVEMENTSERVICES_H
