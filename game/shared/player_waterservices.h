#ifndef PLAYER_WATERSERVICES_H
#define PLAYER_WATERSERVICES_H

#pragma once

#include "playerpawn_component.h"

//-----------------------------------------------------------------------------
// CPlayer_WaterServices, slots 23-25. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CPlayer_WaterServicesVTable : public TBase
{
public:
	// CCSPlayer_WaterServices plays Player.Wade when the pawn enters water; the client
	// starts and stops the underwater ambience here
	virtual void OnWaterLevelChanged( float flOldLevel, float flNewLevel ) = 0;

	// Returns false, never overridden; the client has no such slot. The drowning code skips the
	// air timer when the water does not cover the pawn or when this returns true.
	// CBasePlayer::CanBreatheUnderwater of Source 1 and Dota 2 2018
	virtual bool CanBreatheUnderwater() = 0;

	// Run from Reset. CCSPlayer_WaterServices restarts its drowning state here.
	virtual void ResetWaterState() = 0;
};

#endif // PLAYER_WATERSERVICES_H
