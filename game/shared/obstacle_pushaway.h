//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Entity enumerators of the bots that look for obstacles around them.
//
//=============================================================================//

#ifndef OBSTACLE_PUSHAWAY_H
#define OBSTACLE_PUSHAWAY_H
#ifdef _WIN32
#pragma once
#endif

#include "util_shared.h"

//-----------------------------------------------------------------------------
// Finds breakable entities that are not doors
//-----------------------------------------------------------------------------
class CBotBreakableEnumerator : public CTraceFilterHitEntitiesOnly
{
public:
	CBotBreakableEnumerator( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterHitEntitiesOnly( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

//-----------------------------------------------------------------------------
// Finds func_door and prop_door entities that are closed (not open or opening)
//-----------------------------------------------------------------------------
class CBotDoorEnumerator : public CTraceFilterHitEntitiesOnly
{
public:
	CBotDoorEnumerator( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterHitEntitiesOnly( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

#endif // OBSTACLE_PUSHAWAY_H
