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
#include "steam/isteamugc.h"

#include <functional>

//-----------------------------------------------------------------------------
// A Steam UGC details query for one addon, 112 bytes on Linux, 136 on Windows (std::function is
// 32 bytes in libstdc++, 64 in the MSVC library). Its own virtual destructor, its only virtual
// method, unregisters the call result and releases the query.
//-----------------------------------------------------------------------------
abstract_class CUGCDetailsRequest
{
public:
	virtual ~CUGCDetailsRequest() {}

public:
	// Named by what it holds: the SteamUGCQueryCompleted_t call result of SendQueryUGCRequest; its
	// handler checks that the result belongs to m_hQuery, calls m_OnCompleted and sets m_bCompleted
	CCallResult< CUGCDetailsRequest, SteamUGCQueryCompleted_t > m_callResult;
	// Zeroed by the constructor; nothing reads or writes it after that
	uint64 m_nZeroedAtConstruction;
	// Named by what it holds: the query (CreateQueryUGCDetailsRequest), sent on creation and
	// released by the destructor
	UGCQueryHandle_t m_hQuery;
	// Named by what it does: set when the query completed or could not be sent; CCSAddonManager
	// deletes the completed requests
	bool m_bCompleted;
	// Named by what it holds: the callback of the requester, called with the result
	std::function< void( SteamUGCQueryCompleted_t *, bool ) > m_OnCompleted;
};

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
	// Named by what it holds: the outstanding UGC detail requests (CUGCDetailsRequest::m_bCompleted
	// marks the ones to delete)
	CUtlVector< CUGCDetailsRequest * > m_UGCDetailsRequests;
	// Named by what the code does with it: Shutdown passes it to slot 25 of an engine interface and
	// clears it; nothing in the server sets it
	void *m_pReleasedOnShutdown;
};

#endif // CS_ADDONMANAGER_H
