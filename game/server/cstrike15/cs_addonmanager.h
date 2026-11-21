//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Workshop addon bookkeeping of the CS server: the convar whitelist
//          of addons and the outstanding UGC detail requests.
//
//=============================================================================//

#ifndef CS_ADDONMANAGER_H
#define CS_ADDONMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlvector.h>

// A Steam UGC query (SteamUGCQueryCompleted_t call result) for one addon, 112 bytes; its own
// virtual destructor frees it
class CUGCDetailsRequest;

//-----------------------------------------------------------------------------
// Registered as "CCSAddonManager" (static factory). It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CCSAddonManager : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CCSAddonManager();
	virtual ~CCSAddonManager();

	// Loads the "whitelist_cvars" list of the addon whitelist KV3 file into a static symbol table;
	// returns true
	virtual bool Init() override;
	// Releases m_pReleasedOnShutdown and deletes every request
	virtual void Shutdown() override;
	// Both forward to an engine service (slot 53, and slots 54 and 55 on deactivate)
	GS_EVENT( GameInit );
	GS_EVENT( GameDeactivate );
	// Empty
	GS_EVENT( GameShutdown );
	// For the map spawn group: precaches soundevents/soundevents_addon.vsndevts of the addon map
	GS_EVENT( SpawnGroupPrecache );
	// Deletes the requests that completed
	GS_EVENT( ClientPostUpdate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it holds: the outstanding UGC detail requests (the byte at +72 of a request marks
	// it completed)
	CUtlVector< CUGCDetailsRequest * > m_UGCDetailsRequests;
	// Named by what the code does with it: Shutdown passes it to slot 25 of an engine interface and
	// clears it; nothing in the server sets it
	void *m_pReleasedOnShutdown;
};

#endif // CS_ADDONMANAGER_H
