//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Keeps the lists of the entities that think or simulate this tick.
//
//=============================================================================//

#ifndef SIMTHINKMANAGER_H
#define SIMTHINKMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "spawngrouptypes.h"

#include <entity2/entityidentity.h>
#include <tier0/utlvector.h>

class CEntityInstance;

//-----------------------------------------------------------------------------
// The simulation think manager interface. No virtual destructor. Besides the think list
// (GetSimThinkList, EntityChanged, EntityDeleted) it keeps a second list of entities that
// simulate without a think (the "simulate list"); GameShutdown clears both. The methods of the
// simulate list are named by what they do.
//-----------------------------------------------------------------------------
abstract_class ISimulationThinkManager
{
public:
	// Lets the entity system finish its pending work, then, when bPlayers is set, runs a
	// per-player step on every connected player's pawn
	virtual void PrepareSimulation( bool bPlayers ) = 0;
	// Fills list with the entities whose next think tick has come (sorted)
	virtual void GetSimThinkList( CUtlVector< CEntityHandle > &list ) = 0;
	virtual void GetSimulateList( CUtlVector< CEntityHandle > &list ) = 0;
	// Adds the entity to the think list with nNextThinkTick, or removes it (bRemove, or when the
	// entity no longer thinks)
	virtual void EntityChanged( CEntityInstance *pEntity, int nNextThinkTick, bool bRemove ) = 0;
	// Adds the entity to the simulate list when bSimulate is set, removes it otherwise
	virtual void EntitySimulateChanged( CEntityInstance *pEntity, bool bSimulate ) = 0;
	// Removes the entity from both lists
	virtual void EntityDeleted( CEntityInstance *pEntity ) = 0;
};

//-----------------------------------------------------------------------------
// Registered as "CSimThinkManager" (static factory). 131152 bytes on both platforms.
//-----------------------------------------------------------------------------
class CSimThinkManager : public CAutoGameSystem, public ISimulationThinkManager
{
public:
	DECLARE_GAME_SYSTEM();

	// The entity and its world group, whose current tick the think tick is compared with
	struct simthinkentry_t
	{
		CEntityHandle m_hEntity;
		int m_nNextThinkTick;
		WorldGroupId_t m_hWorldGroupId;
	};

	// The index of an entity in each list, 0xFFFF when it is not in it.
	struct EntInfoIndex_t
	{
		unsigned short m_nSimThinkIndex;
		unsigned short m_nSimulateIndex;
	};

	virtual ~CSimThinkManager();

	// Empties both lists and resets m_entinfoIndex
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// ISimulationThinkManager
	virtual void PrepareSimulation( bool bPlayers ) override;
	virtual void GetSimThinkList( CUtlVector< CEntityHandle > &list ) override;
	virtual void GetSimulateList( CUtlVector< CEntityHandle > &list ) override;
	virtual void EntityChanged( CEntityInstance *pEntity, int nNextThinkTick, bool bRemove ) override;
	virtual void EntitySimulateChanged( CEntityInstance *pEntity, bool bSimulate ) override;
	virtual void EntityDeleted( CEntityInstance *pEntity ) override;

public:
	// By entity index
	EntInfoIndex_t m_entinfoIndex[ 32768 ];
	CUtlVector< simthinkentry_t > m_simThinkList;
	// Named by what it holds
	CUtlVector< CEntityHandle > m_simulateList;
};

#endif // SIMTHINKMANAGER_H
