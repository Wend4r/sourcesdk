//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Runs the precache callbacks registered for a spawn group.
//
//=============================================================================//

#ifndef RESOURCEPRECACHER_H
#define RESOURCEPRECACHER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "spawngrouptypes.h"

#include <tier0/utlmap.h>
#include <tier0/utldelegateimpl.h>

//-----------------------------------------------------------------------------
// Registered as "CResourcePrecacherGameSystem" (static factory). 64 bytes on both platforms.
//-----------------------------------------------------------------------------
class CResourcePrecacherGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	typedef CUtlDelegate< void( const EventSpawnGroupPrecache_t & ) > SpawnGroupPrecacheDelegate_t;

	virtual ~CResourcePrecacherGameSystem();

	// Calls the delegate registered for the spawn group of the message once and removes it
	GS_EVENT( SpawnGroupPrecache );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// The precache callback of each spawn group
	CUtlMap< SpawnGroupHandle_t, SpawnGroupPrecacheDelegate_t, int > m_SpawnGroupPrecacheDelegates;
};

#endif // RESOURCEPRECACHER_H
