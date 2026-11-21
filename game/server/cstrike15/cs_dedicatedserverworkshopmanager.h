//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Workshop maps and addons of a dedicated server: detail queries,
//          downloads ("mapgroup workshop") and the addon path resolver.
//
//=============================================================================//

#ifndef CS_DEDICATEDSERVERWORKSHOPMANAGER_H
#define CS_DEDICATEDSERVERWORKSHOPMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "appframework/iugcaddonpathresolver.h"

#include <tier0/utlmap.h>

#include "steam/steam_api.h"
#include "steam/isteamugc.h"

//-----------------------------------------------------------------------------
// Registered as "DedicatedServerWorkshopManager" (static factory, 256 bytes). No source names its
// members; they are named by what the code does with them. It adds no virtual methods to
// IGameSystem; on Linux the GetFullAddonPathFromAddonName override also takes slot 66 of the
// primary table.
//-----------------------------------------------------------------------------
class CDedicatedServerWorkshopManager : public CAutoGameSystem, public IUGCAddonPathResolver
{
public:
	DECLARE_GAME_SYSTEM();

	// The value of m_mapAddonPaths (216 bytes, operator new)
	struct AddonPathInfo_t;

	CDedicatedServerWorkshopManager();
	virtual ~CDedicatedServerWorkshopManager();

	// Installs itself as the application's UGC addon path resolver (dedicated server only); returns true
	virtual bool Init() override;
	// Frees the details of m_mapUGCDetails and the entries of m_mapAddonPaths
	virtual void Shutdown() override;
	// Drops the finished details, sends the query for m_unQueryFileID, processes m_mapPendingMaps
	GS_EVENT( ServerPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual bool GetFullAddonPathFromAddonName( const char *pszAddonName, CBufferString *pOutPath ) const override;

private:
	void OnItemInstalled( ItemInstalled_t *pParam );
	void OnSteamServersConnected( SteamServersConnected_t *pParam );

public:
	// ItemInstalled_t (3406) and SteamServersConnected_t (101), game server callbacks
	CCallback< CDedicatedServerWorkshopManager, ItemInstalled_t, true > m_CallbackItemInstalled;
	CCallback< CDedicatedServerWorkshopManager, SteamServersConnected_t, true > m_CallbackSteamServersConnected;
	// The Steam UGC details received per published file (SteamUGCDetails_t, 9784 bytes each)
	CUtlMap< PublishedFileId_t, SteamUGCDetails_t *, int > m_mapUGCDetails;
	// The local path of each downloaded addon (GetFullAddonPathFromAddonName)
	CUtlMap< PublishedFileId_t, AddonPathInfo_t *, int > m_mapAddonPaths;
	// The workshop maps waiting for their download ("mapgroup workshop"); only the keys are read
	CUtlMap< PublishedFileId_t, void *, int > m_mapPendingMaps;
	// The workshop map being downloaded / switched to
	PublishedFileId_t m_unCurrentMapFileID;
	// The file whose UGC details ServerPostSimulate queries next (CreateQueryUGCDetailsRequest), then 0
	PublishedFileId_t m_unQueryFileID;
	// Set when the server runs with workshop content; gates ServerPostSimulate and the resolver
	bool m_bWorkshopEnabled;
};

#endif // CS_DEDICATEDSERVERWORKSHOPMANAGER_H
