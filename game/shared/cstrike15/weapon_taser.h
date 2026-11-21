//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filter of the taser.
//
//=============================================================================//

#ifndef CSTRIKE15_WEAPON_TASER_H
#define CSTRIKE15_WEAPON_TASER_H
#ifdef _WIN32
#pragma once
#endif

#include "../util_shared.h"

//-----------------------------------------------------------------------------
// Skips the attacker (the first pass entity) and its teammates
//-----------------------------------------------------------------------------
class CTraceFilterTaserIgnoreTeammates : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterTaserIgnoreTeammates( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

#endif // CSTRIKE15_WEAPON_TASER_H
