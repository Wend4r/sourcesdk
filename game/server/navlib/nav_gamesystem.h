//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that loads a navigation mesh for every spawn group.
//
//=============================================================================//

#ifndef NAV_GAMESYSTEM_H
#define NAV_GAMESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlmap.h"
#include "igamesystem.h"
#include "spawngrouptypes.h"
#include "nav_mesh.h"

// The name its factory is registered under; the factory reallocates the system every server
// start and keeps it in a global of its own.
#define NAV_GAME_SYSTEM_NAME "NavGameSystem"

class CNavSpaceGame;

//-----------------------------------------------------------------------------
// Registered as "NavGameSystem" (reallocating factory), 72 bytes on both platforms. A direct
// child of IGameSystem with no own virtual methods.
//-----------------------------------------------------------------------------
class CNavGameSystem : public IGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Created when a spawn group with nav data starts loading
	struct NavInfoForSpawnGroup_t
	{
		// The spawn group's own mesh, or m_pNavMeshForSpawnGroupsWithNoNav when none was created.
		CNavMesh *m_pNavMesh;
		// The mesh that was active when this one loaded.
		CNavMesh *m_pPreviousNavMesh;
		CNavSpaceGame *m_pNavSpace;
		// Loaded from the map's .navflowmap file; no class name survives for it.
		void *m_pFlowMap;
		// Named by what it does: when set, the spawn group becoming the active one makes the active
		// nav space rebuild (its third virtual with false) and clears the flag. No writer of true
		// was found.
		bool m_bRebuildNavSpaceOnActivate;
	};

	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );
	// Loads the nav mesh of the spawn group (LoadNavForSpawnGroup)
	GS_EVENT( PreSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupUnload );
	// Makes the mesh of the new active spawn group the one bots use
	GS_EVENT( ActiveSpawnGroupChanged );
	GS_EVENT( RestoreGame );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~CNavGameSystem();

public:
	// Every loaded spawn group that has nav data. The active one's mesh is the one bots use.
	CUtlMap< SpawnGroupHandle_t, NavInfoForSpawnGroup_t *, int > m_NavInfoForSpawnGroup;
	// Used by spawn groups without nav data; null when nav is disabled.
	CNavMesh *m_pNavMeshForSpawnGroupsWithNoNav;
};

typedef CNavGameSystem::NavInfoForSpawnGroup_t NavInfoForSpawnGroup_t;

#endif // NAV_GAMESYSTEM_H
