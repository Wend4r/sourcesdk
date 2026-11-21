//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Tells registered entities when a player sees them for the first time.
//
//=============================================================================//

#ifndef CVISIBILITYMONITOR_H
#define CVISIBILITYMONITOR_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <entity2/entityidentity.h>
#include <tier0/utlvector.h>

class CBaseEntity;
class CBasePlayerPawn;

typedef bool ( *VisibilityMonitorCallback )( CBaseEntity *pVisibleEntity, CBasePlayerPawn *pViewingPlayer );
typedef bool ( *VisibilityMonitorEvaluator )( CBaseEntity *pVisibleEntity, CBasePlayerPawn *pViewingPlayer );

struct visibility_target_t
{
	CEntityHandle entity;
	float minDistSqr;
	// Bit per player that has seen the entity
	int memory;
	VisibilityMonitorCallback pfnCallback;
	VisibilityMonitorEvaluator pfnEvaluator;
};

//-----------------------------------------------------------------------------
// Registered as "VisibilityMonitor" (static factory). 72 bytes on both platforms.
//-----------------------------------------------------------------------------
class CVisibilityMonitor : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CVisibilityMonitor();

	virtual bool Init() override;
	virtual void Shutdown() override;
	GS_EVENT( GameInit );
	GS_EVENT( PostSpawnGroupLoad );
	GS_EVENT( PreSpawnGroupUnload );
	// Tests up to m_iMaxEntitiesPerThink entities (m_iMaxTracesPerThink traces) every
	// m_flPollFrequency seconds, starting at m_iStartElement
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual bool EntityIsVisibleToPlayer( const visibility_target_t &target, CBasePlayerPawn *pPlayer, int *numTraces );

public:
	CUtlVector< visibility_target_t > m_Entities;
	float m_flPollFrequency;
	float m_flTimeNextPoll;
	int m_iMaxTracesPerThink;
	int m_iMaxEntitiesPerThink;
	int m_iStartElement;
};

#endif // CVISIBILITYMONITOR_H
