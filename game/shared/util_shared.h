//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared trace filters.
//
//=============================================================================//

#ifndef UTIL_SHARED_H
#define UTIL_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "gametrace.h"
#include <tier0/utlvector.h>

class CBaseEntity;

// An extra per-entity check of a trace: the entity and the m_nInteractsWith of the query
typedef bool ( *ShouldHitFunc_t )( CBaseEntity *pEntity, uint64 nInteractsWith );

//-----------------------------------------------------------------------------
// A trace filter that asks ShouldHitEntity for every entity it meets (m_bIterateEntities set).
// No fields of its own; 72 bytes on Windows, 64 on Linux like CTraceFilter.
//-----------------------------------------------------------------------------
class CTraceFilterSlowWithCallback : public CTraceFilter
{
public:
	CTraceFilterSlowWithCallback( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilter( nInteractsWith, nCollisionGroup, true )
	{
	}
};

//-----------------------------------------------------------------------------
// Queries only the entity bodies (keyframed, dynamic and locatable objects, no static world)
//-----------------------------------------------------------------------------
class CTraceFilterHitEntitiesOnly : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterHitEntitiesOnly( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
		m_nObjectSetMask = RNQUERY_OBJECTS_KEYFRAMED | RNQUERY_OBJECTS_DYNAMIC | RNQUERY_OBJECTS_LOCATABLE;
	}
};

//-----------------------------------------------------------------------------
// Skips player pawns, player controllers and NPCs
//-----------------------------------------------------------------------------
class CTraceFilterSkipPlayersAndNPCs : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterSkipPlayersAndNPCs( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

//-----------------------------------------------------------------------------
// Skips players and NPCs and every entity parented to one (walks the move parents)
//-----------------------------------------------------------------------------
class CTraceFilterSkipPlayersAndNPCsAndChildren : public CTraceFilterSkipPlayersAndNPCs
{
public:
	CTraceFilterSkipPlayersAndNPCsAndChildren( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSkipPlayersAndNPCs( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

//-----------------------------------------------------------------------------
// The filter of the anim graph traces: copies the query attributes of the request
//-----------------------------------------------------------------------------
class CAnimGraphTraceFilter : public CTraceFilterSkipPlayersAndNPCsAndChildren
{
public:
	CAnimGraphTraceFilter( const RnQueryAttr_t &attr )
	{
		*static_cast< RnQueryAttr_t * >( this ) = attr;
	}
};

//-----------------------------------------------------------------------------
// Skips null entities, NPCs and player pawns and controllers
//-----------------------------------------------------------------------------
class CTraceFilterNoNPCsOrPlayer : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterNoNPCsOrPlayer( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

//-----------------------------------------------------------------------------
// Line of sight: skips the two pass entities through the query attributes only, ShouldHitEntity
// is the one of CTraceFilter (m_bIterateEntities stays false)
//-----------------------------------------------------------------------------
class CTraceFilterLOS : public CTraceFilter
{
public:
	CTraceFilterLOS( CEntityInstance *pPassEntity1, CEntityInstance *pPassEntity2, uint64 nInteractsWith, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilter( pPassEntity1, pPassEntity2, nullptr, nullptr, 0, 0, nInteractsWith, nCollisionGroup, false )
	{
	}
};

//-----------------------------------------------------------------------------
// Skips the entities of a list. 96 bytes on Windows, 88 on Linux.
//-----------------------------------------------------------------------------
class CTraceFilterList : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterList( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
		m_bIterateEntities = false;
	}

	void AddEntityToIgnore( CBaseEntity *pEntity )
	{
		m_bIterateEntities = true;
		m_PassEntities.AddToTail( pEntity );
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;

public:
	CUtlVector< CBaseEntity * > m_PassEntities;
};

//-----------------------------------------------------------------------------
// Used when an entity pushes others: the query attributes come from the pusher's collision
// property. 88 bytes on Windows, 80 on Linux.
//-----------------------------------------------------------------------------
class CTraceFilterEntityPush : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterEntityPush( CBaseEntity *pEntity );

	// Returns true
	bool ShouldHitEntity( CEntityInstance *pEntity ) override;

public:
	// The topmost move parent of m_pEntity
	CBaseEntity *m_pRootParent;
	// Named by what it holds: the pushing entity
	CBaseEntity *m_pEntity;
};

//-----------------------------------------------------------------------------
// Used when an entity is swept along a path; built like CTraceFilterEntityPush. 88 bytes on
// Windows, 80 on Linux.
//-----------------------------------------------------------------------------
class CTraceFilterEntitySweep : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterEntitySweep( CBaseEntity *pEntity );

	// Returns true
	bool ShouldHitEntity( CEntityInstance *pEntity ) override;

public:
	// The topmost move parent of m_pEntity
	CBaseEntity *m_pRootParent;
	// Named by what it holds: the swept entity
	CBaseEntity *m_pEntity;
};

#endif // UTIL_SHARED_H
