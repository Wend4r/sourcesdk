//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filter of the flashbang projectile.
//
//=============================================================================//

#ifndef CSTRIKE15_FLASHBANG_PROJECTILE_H
#define CSTRIKE15_FLASHBANG_PROJECTILE_H
#ifdef _WIN32
#pragma once
#endif

#include "../util_shared.h"

//-----------------------------------------------------------------------------
// Whether the flash reaches a target: weapons, grenades and hostages do not block it. 80 bytes on
// Windows, 72 on Linux.
//-----------------------------------------------------------------------------
class CTraceFilterNoPlayersAndFlashbangPassableAnims : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterNoPlayersAndFlashbangPassableAnims( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup ), mTargetEntity( nullptr )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;

public:
	// The entity whose visibility is tested; the game neither sets nor reads it
	const CEntityInstance *mTargetEntity;
};

#endif // CSTRIKE15_FLASHBANG_PROJECTILE_H
