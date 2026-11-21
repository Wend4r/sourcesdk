//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that preloads an adjacent map around a landmark
//			(the "adjacent_preload nextmap landmarkname" command; factory
//			"PreloaderGameSystem,"). No Valve path names this header.
//
//=============================================================================//

#ifndef PRELOADER_GAMESYSTEM_H
#define PRELOADER_GAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entityhandle.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "world_composition_reference.h"

//-----------------------------------------------------------------------------
// Static instance, 48 bytes on both platforms. The names are named by what the
// code does with the members.
//-----------------------------------------------------------------------------
class CPreloaderGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// For each preload: logs when its resources are loaded and when it is ready to spawn
	// entities, activates its spawn group once the engine allows it (moving the entities in
	// the transition volume and teleporting the players by the landmark offset) and drops it
	GS_EVENT( ServerPreEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// 128 bytes
	struct Preload_t
	{
		CUtlString m_MapName;
		CUtlString m_LandmarkName;
		// This map's landmark entity
		CEntityHandle m_hLandmark;
		// Time stamp counter when the preload started, then the ticks the resources took
		uint64 m_nLoadTicks;
		// Zeroed when the preload starts, read nowhere
		int m_nZeroedAtStart;
		// Zeroed with the entry, neither written nor read afterwards
		int m_nNeverAccessed;
		// Set when the preload starts, cleared once the resources are loaded
		bool m_bLoadingResources;
		// Set once the spawn group can spawn its entities
		bool m_bReadyToSpawn;
		CEntityLumpRequest m_Request;
	};

	CUtlVector< Preload_t > m_Preloads;
};

#endif // PRELOADER_GAMESYSTEM_H
