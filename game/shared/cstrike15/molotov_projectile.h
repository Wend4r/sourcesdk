//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filter of the molotov projectile.
//
//=============================================================================//

#ifndef CSTRIKE15_MOLOTOV_PROJECTILE_H
#define CSTRIKE15_MOLOTOV_PROJECTILE_H
#ifdef _WIN32
#pragma once
#endif

#include "../util_shared.h"

//-----------------------------------------------------------------------------
// Skips grenade projectiles (CBaseCSGrenadeProjectile). The class name is spelled so in the game.
//-----------------------------------------------------------------------------
class CTraceFilterIngnoreGrenades : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterIngnoreGrenades( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

#endif // CSTRIKE15_MOLOTOV_PROJECTILE_H
