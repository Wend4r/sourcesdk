//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filters of the pusher physics.
//
//=============================================================================//

#ifndef PHYSICS_MAIN_SHARED_H
#define PHYSICS_MAIN_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "util_shared.h"

//-----------------------------------------------------------------------------
// Used while a pusher moves: hits nothing while the root parent is in the way, skips dynamic
// physics objects otherwise. 80 bytes on Windows, 72 on Linux.
//-----------------------------------------------------------------------------
class CTraceFilterPushMove : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterPushMove( CBaseEntity *pRootParent, uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup ), m_pRootParent( pRootParent )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;

public:
	CBaseEntity *m_pRootParent;
};

//-----------------------------------------------------------------------------
// The last check of a push: skips dynamic physics objects and, when asked, the teammates of the
// pass entity. 80 bytes on Windows; 64 on Linux, where m_bIgnoreTeammates lives in the tail
// padding of CTraceFilter (offset 59).
//-----------------------------------------------------------------------------
class CTraceFilterPushFinal : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterPushFinal( bool bIgnoreTeammates, uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup ), m_bIgnoreTeammates( bIgnoreTeammates )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;

public:
	bool m_bIgnoreTeammates;
};

#endif // PHYSICS_MAIN_SHARED_H
