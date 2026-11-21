//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The server's Steamworks game stats uploader.
//
//=============================================================================//

#ifndef STEAMWORKS_GAMESTATS_SERVER_H
#define STEAMWORKS_GAMESTATS_SERVER_H
#ifdef _WIN32
#pragma once
#endif

#include "steamworks_gamestats.h"

//-----------------------------------------------------------------------------
// Not registered itself (YouForgot_DECLARE_GAME_SYSTEM is still pure); adds no fields.
//-----------------------------------------------------------------------------
class CSteamWorksGameStatsServer : public CSteamWorksGameStatsUploader
{
public:
	virtual ~CSteamWorksGameStatsServer();

	virtual void WriteSessionRow() override;

protected:
	// k_EGameStatsAccountType_SteamGameServer, or k_EGameStatsAccountType_Steam when the engine
	// says the server is not a dedicated one
	virtual EGameStatsAccountType GetGameStatsAccountType() override;
	virtual void AddSessionIDsToTable( int iTableID, int iPlayerSlot ) override;
};

#endif // STEAMWORKS_GAMESTATS_SERVER_H
