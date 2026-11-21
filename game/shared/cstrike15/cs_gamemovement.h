//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filters of the CS player movement.
//
//=============================================================================//

#ifndef CSTRIKE15_CS_GAMEMOVEMENT_H
#define CSTRIKE15_CS_GAMEMOVEMENT_H
#ifdef _WIN32
#pragma once
#endif

#include "../util_shared.h"

//-----------------------------------------------------------------------------
// A plain filter of the CS player code: ShouldHitEntity is the one of CTraceFilter, no fields of
// its own
//-----------------------------------------------------------------------------
class CCSTraceFilterSimple : public CTraceFilter
{
public:
	CCSTraceFilterSimple( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT, bool bIterateEntities = true )
		: CTraceFilter( nInteractsWith, nCollisionGroup, bIterateEntities )
	{
	}
};

//-----------------------------------------------------------------------------
// The movement trace of a player pawn (the first pass entity): in the player movement collision
// group it lets players of the same team pass through each other
//-----------------------------------------------------------------------------
class CTraceFilterPlayerMovementCS : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterPlayerMovementCS( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

//-----------------------------------------------------------------------------
// Whether a player's head hits another player: skips players of the same team (team bits of
// m_nInteractsWith)
//-----------------------------------------------------------------------------
class CTraceFilterForPlayerHeadCollision : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterForPlayerHeadCollision( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

#endif // CSTRIKE15_CS_GAMEMOVEMENT_H
