//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filters and pushed entities of the pusher physics.
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

//-----------------------------------------------------------------------------
// The entities a pusher moves in one push; CCSPhysicsPushEntities overrides it and adds no member
// (one static object, 96 bytes on both platforms). Three slots on both platforms, no virtual
// destructor.
//-----------------------------------------------------------------------------
abstract_class CPhysicsPushedEntities
{
public:
	struct RotatingPushMove_t;

	// A pusher and where it started, 24 bytes
	struct PhysicsPusherInfo_t
	{
		CBaseEntity *m_pEntity;
		Vector m_vecStartAbsOrigin;
	};

	// A pushed entity and the state of its push, 240 bytes
	struct PhysicsPushedInfo_t
	{
		CBaseEntity *m_pEntity;
		Vector m_vecStartAbsOrigin;
		CGameTrace m_Trace;
		bool m_bBlocked;
		bool m_bPusherIsGround;
	};

	virtual bool SpeculativelyCheckRotPush( const RotatingPushMove_t &rotPushMove, CBaseEntity *pRoot ) = 0;
	virtual bool SpeculativelyCheckLinearPush( const Vector &vecAbsPush ) = 0;
	virtual void FinishRotPushedEntity( CBaseEntity *pPushedEntity, const RotatingPushMove_t &rotPushMove ) = 0;

public:
	// Reserved for 8 at construction
	CUtlVector< PhysicsPusherInfo_t > m_rgPusher;
	// Reserved for 32 at construction
	CUtlVector< PhysicsPushedInfo_t > m_rgMoved;
	// The index in m_rgMoved of the entity that blocked the push, -1 for none
	int m_nBlocker;
	bool m_bIsUnblockableByPlayer;
	Vector m_rootPusherStartLocalOrigin;
	QAngle m_rootPusherStartLocalAngles;
	float m_rootPusherStartLocaltime;
	// -1 at construction
	float m_flMoveTime;
};

#endif // PHYSICS_MAIN_SHARED_H
