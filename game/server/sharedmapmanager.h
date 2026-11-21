//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Reference counts the spawn groups of shared maps (shared_environment).
//
//=============================================================================//

#ifndef SHAREDMAPMANAGER_H
#define SHAREDMAPMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "spawngrouptypes.h"

#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CSharedMapManager" (reallocating factory). 48 bytes on both platforms. No source
// has the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CSharedMapManager : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	struct SharedMap_t
	{
		CUtlString m_MapName;
		SpawnGroupHandle_t m_hSpawnGroup;
		// The entities that use the map; the spawn group is unloaded when it drops to 0
		int m_nRefCount;
	};

	virtual ~CSharedMapManager();

	// Empties m_SharedMaps
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< SharedMap_t > m_SharedMaps;
};

#endif // SHAREDMAPMANAGER_H
