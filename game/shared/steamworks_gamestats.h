//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Uploads game stats through the (retired) Steamworks game stats interface.
//
//=============================================================================//

#ifndef STEAMWORKS_GAMESTATS_H
#define STEAMWORKS_GAMESTATS_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

// CGameEventListener
#include <vscript/ccscript.h>

#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <steam/steam_api_common.h>
#include <steam/steamtypes.h>
#include <steam/steamclientpublic.h>

class KeyValues;
class ConVarRef;
class ISteamGameStats;

// The Steamworks SDK no longer has isteamgamestats.h; the declarations below are those of an
// older Steamworks SDK (public/steam/isteamgamestats.h).
enum EGameStatsAccountType
{
	k_EGameStatsAccountType_Steam = 1,				// ullAccountID is a 64-bit SteamID for a player
	k_EGameStatsAccountType_Xbox = 2,				// ullAccountID is a 64-bit XUID
	k_EGameStatsAccountType_SteamGameServer = 3,	// ullAccountID is a 64-bit SteamID for a game server
};

struct GameStatsSessionIssued_t
{
	enum { k_iCallback = 2000 + 1 };	// k_iSteamGameStatsCallbacks + 1

	uint64 m_ulSessionID;
	EResult m_eResult;
	bool m_bCollectingAny;
	bool m_bCollectingDetails;
};

struct GameStatsSessionClosed_t
{
	enum { k_iCallback = 2000 + 2 };	// k_iSteamGameStatsCallbacks + 2

	uint64 m_ulSessionID;
	EResult m_eResult;
};

// The length of the three name buffers: MAX_PATH of the platform the game was built for. sourcesdk's
// MAX_PATH is 260 everywhere, the Linux game uses 4096 (PATH_MAX), so the buffers really differ.
#ifdef _WIN32
#define STEAMWORKS_GAMESTATS_MAX_PATH 260
#else
#define STEAMWORKS_GAMESTATS_MAX_PATH 4096
#endif

typedef CUtlVector< KeyValues * > KeyValueStatList;

struct ClientServerSession_t
{
	uint64 m_ServerSessionID;
	RTime32 m_ConnectTime;
	RTime32 m_DisconnectTime;
};

//-----------------------------------------------------------------------------
// Not registered itself (abstract: GetGameStatsAccountType and AddSessionIDsToTable are pure, and
// so is YouForgot_DECLARE_GAME_SYSTEM: only CCSGOSteamWorksGameStatsServer has the macro). 1032
// bytes on Windows; on Linux MAX_PATH is 4096 (STEAMWORKS_GAMESTATS_MAX_PATH), which makes it 12544 bytes.
//-----------------------------------------------------------------------------
class CSteamWorksGameStatsUploader : public CAutoGameSystem, public CGameEventListener
{
public:
	virtual ~CSteamWorksGameStatsUploader();

	virtual bool Init() override;
	virtual void PostInit() override;
	virtual void Shutdown() override;
	GS_EVENT( ServerPreEntityThink );
	GS_EVENT( ServerPostEntityThink );
	GS_EVENT( ServerPreClientUpdate );
	GS_EVENT( SaveGame );
	GS_EVENT( RestoreGame );

	virtual void EndSession();
	virtual void WriteSessionRow();
	// Empty
	virtual void SafeRemoveIfDesired();
	// Returns true
	virtual bool IsPerFrame();

	virtual void FireGameEvent( IGameEvent *event ) override;

protected:
	virtual EGameStatsAccountType GetGameStatsAccountType() = 0;
	// The table and the player slot whose session IDs are written
	virtual void AddSessionIDsToTable( int iTableID, int iPlayerSlot ) = 0;
	virtual void Reset();

public:
	CCallResult< CSteamWorksGameStatsUploader, GameStatsSessionIssued_t > m_CallbackSteamSessionInfoIssued;
	CCallResult< CSteamWorksGameStatsUploader, GameStatsSessionClosed_t > m_CallbackSteamSessionInfoClosed;

	ISteamGameStats *m_SteamWorksInterface;
	uint64 m_UserID;
	uint32 m_iAppID;
	uint32 m_iServerIP;
	char m_pzServerIP[ STEAMWORKS_GAMESTATS_MAX_PATH ];
	char m_pzMapStart[ STEAMWORKS_GAMESTATS_MAX_PATH ];
	char m_pzHostName[ STEAMWORKS_GAMESTATS_MAX_PATH ];
	RTime32 m_StartTime;
	RTime32 m_EndTime;
	bool m_bPassword;
	uint64 m_SessionID;
	bool m_SessionIDRequestUnsent;
	bool m_SessionIDRequestPending;
	bool m_bCollectingAny;
	bool m_bCollectingDetails;
	bool m_ServiceTicking;
	float m_LastServiceTick;
	bool m_UploadedStats;
	KeyValueStatList m_StatsToSend;
	ClientServerSession_t m_ActiveSession;
	int m_iServerConnectCount;
	CUtlString m_sSessionConVarName;
	ConVarRef *m_pSessionConVar;
};

#endif // STEAMWORKS_GAMESTATS_H
