//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Spawn groups the CS server creates or adopts to precache resources
//          ("cs_spawn_group_precache").
//
//=============================================================================//

#ifndef CS_SPAWNGROUPSYSTEM_H
#define CS_SPAWNGROUPSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CCSSpawnGroupSystem" (static factory). No source names its members; they are
// named by what the code does with them. It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CCSSpawnGroupSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// One spawn group (72 bytes, operator new)
	struct SpawnGroupInfo_t
	{
		SpawnGroupHandle_t m_hSpawnGroup;
		// 2 for a spawn group this system created ("cs_spawn_group_precache"), 0 for one it adopted
		int m_nCreationType;
		// Freed on GameShutdown; set nowhere in the server
		CUtlString m_sName;
		// Precached into the spawn group's manifest in SpawnGroupPrecache
		CUtlVector< CUtlString > m_PrecacheResources;
		// Hashed (lower case) and precached by token in SpawnGroupPrecache
		CUtlVector< CUtlString > m_PrecacheTokens;
		// Zeroed on creation, read nowhere in the server
		bool m_bZeroedOnCreation;
	};

	CCSSpawnGroupSystem();
	virtual ~CCSSpawnGroupSystem();

	// Deletes every SpawnGroupInfo_t
	GS_EVENT( GameShutdown );
	// For a spawn group of m_SpawnGroups: precaches its resources and tokens
	GS_EVENT( SpawnGroupPrecache );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< SpawnGroupInfo_t * > m_SpawnGroups;
};

#endif // CS_SPAWNGROUPSYSTEM_H
