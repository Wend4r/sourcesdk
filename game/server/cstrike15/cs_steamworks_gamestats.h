//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The CS2 server game stats uploader.
//
//=============================================================================//

#ifndef CSTRIKE15_CS_STEAMWORKS_GAMESTATS_H
#define CSTRIKE15_CS_STEAMWORKS_GAMESTATS_H
#ifdef _WIN32
#pragma once
#endif

#include "steamworks_gamestats_server.h"

//-----------------------------------------------------------------------------
// Registered as "CCSGOSteamWorksGameStatsServer" (static factory). 1040 bytes on Windows, 12552 on
// Linux (MAX_PATH in the base). It adds DECLARE_GAME_SYSTEM and the factory forwards to its base.
//-----------------------------------------------------------------------------
class CCSGOSteamWorksGameStatsServer : public CSteamWorksGameStatsServer
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CCSGOSteamWorksGameStatsServer();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // CSTRIKE15_CS_STEAMWORKS_GAMESTATS_H
