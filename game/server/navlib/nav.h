//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filters of the navigation mesh generation.
//
//=============================================================================//

#ifndef NAV_H
#define NAV_H
#ifdef _WIN32
#pragma once
#endif

#include "../../shared/util_shared.h"

// Bits of CTraceFilterWalkableEntities::m_flags: which entities count as walkable (not hit)
enum WalkThruFlags_t
{
	WALK_THRU_PROP_DOORS = 0x01,
	WALK_THRU_FUNC_DOORS = 0x02,
	WALK_THRU_DOORS = ( WALK_THRU_PROP_DOORS | WALK_THRU_FUNC_DOORS ),
	WALK_THRU_BREAKABLES = 0x04,
	WALK_THRU_TOGGLE_BRUSHES = 0x08,
	WALK_THRU_EVERYTHING = ( WALK_THRU_DOORS | WALK_THRU_BREAKABLES | WALK_THRU_TOGGLE_BRUSHES ),
};

//-----------------------------------------------------------------------------
// Ignores players, NPCs and the entities the m_flags make walkable. 80 bytes on Windows; 64 on
// Linux, where m_flags lives in the tail padding of CTraceFilter (offset 60).
//-----------------------------------------------------------------------------
class CTraceFilterWalkableEntities : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterWalkableEntities( unsigned int flags, uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup ), m_flags( flags )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;

public:
	unsigned int m_flags; // WalkThruFlags_t
};

//-----------------------------------------------------------------------------
// Like CTraceFilterWalkableEntities, and also never hits prop doors and func_breakable
//-----------------------------------------------------------------------------
class CTraceFilterGroundEntities : public CTraceFilterWalkableEntities
{
public:
	CTraceFilterGroundEntities( unsigned int flags, uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterWalkableEntities( flags, nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

#endif // NAV_H
