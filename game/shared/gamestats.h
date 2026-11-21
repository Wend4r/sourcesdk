//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The basic game stats (CBaseGameStats) and the game system that
//          drives them (CBaseGameStats_Driver).
//
//=============================================================================//

#ifndef GAMESTATS_H
#define GAMESTATS_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "../server/cstrike15/usercmd.h"

#include <tier0/utldict.h>
#include <tier0/utlstring.h>

class CBaseEntity;
class CBasePlayerPawn;
class CTakeDamageInfo;
class CPropVehicleDriveable;
class CUtlBuffer;
class KeyValues;
class CGamestatsData;
class Vector;

enum StatSendType_t
{
	STATSEND_LEVELSHUTDOWN,
	STATSEND_APPSHUTDOWN,
	STATSEND_NOTENOUGHPLAYERS,
};

// 40 bytes
struct BasicGameStatsRecord_t
{
	int m_nCount;
	int m_nSeconds;
	int m_nCommentary;
	int m_nHDR;
	int m_nCaptions;
	int m_nSkill[ 3 ];
	bool m_bSteam;
	bool m_bCyberCafe;
	int m_nDeaths;
};

// 96 bytes (the dictionary is indexed by int in CS2: 64-byte tree nodes)
struct BasicGameStats_t
{
	int m_nSecondsToCompleteGame;
	BasicGameStatsRecord_t m_Summary;
	CUtlDict< BasicGameStatsRecord_t, int > m_MapTotals;
	bool m_bSteam;
	bool m_bCyberCafe;
	int m_nDXLevel;
};

//-----------------------------------------------------------------------------
// 112 bytes. 54 virtual methods and no destructor in the table.
//-----------------------------------------------------------------------------
class CBaseGameStats
{
public:
	virtual bool UseOldFormat();
	virtual bool AddDataForSend( KeyValues *pKV, StatSendType_t sendType );
	virtual bool ShouldSendDataOnLevelShutdown();
	virtual bool ShouldSendDataOnAppShutdown();
	// New in CS2, named by what it does: returns "CS2ClientPerfData"
	virtual const char *GetPerfDataName();

	virtual void Event_Init();
	virtual void Event_Shutdown();
	virtual void Event_MapChange( const char *szOldMapName, const char *szNewMapName );
	virtual void Event_LevelInit();
	virtual void Event_LevelShutdown( float flElapsed );
	virtual void Event_SaveGame();
	virtual void Event_LoadGame();

	virtual CBaseGameStats *OnInit( CBaseGameStats *pCurrentGameStats, const char *gamedir );

	virtual void Clear();

	virtual bool StatTrackingEnabledForMod();
	virtual bool HaveValidData();
	virtual bool ShouldTrackStandardStats();

	virtual const char *GetStatSaveFileName();
	virtual const char *GetStatUploadRegistryKeyName();

	virtual bool UserPlayedAllTheMaps();

	virtual void Event_PlayerKilled( CBasePlayerPawn *pPlayer, const CTakeDamageInfo &info );
	virtual void Event_PlayerConnected( CBasePlayerPawn *pBasePlayer );
	virtual void Event_PlayerDisconnected( CBasePlayerPawn *pBasePlayer );
	virtual void Event_PlayerDamage( CBasePlayerPawn *pBasePlayer, const CTakeDamageInfo &info );
	virtual void Event_PlayerKilledOther( CBasePlayerPawn *pAttacker, CBaseEntity *pVictim, const CTakeDamageInfo &info );
	virtual void Event_Credits();
	virtual void Event_Commentary();
	virtual void Event_CrateSmashed();
	virtual void Event_Punted( CBaseEntity *pObject );
	virtual void Event_PlayerTraveled( CBasePlayerPawn *pBasePlayer, float distanceInInches, bool bInVehicle, bool bSprinting );
	virtual void Event_WeaponFired( CBasePlayerPawn *pShooter, bool bPrimary, const char *pchWeaponName );
	virtual void Event_WeaponHit( CBasePlayerPawn *pShooter, bool bPrimary, const char *pchWeaponName, const CTakeDamageInfo &info );
	virtual void Event_FlippedVehicle( CBasePlayerPawn *pDriver, CPropVehicleDriveable *pVehicle );
	virtual void Event_PreSaveGameLoaded( const char *pSaveName, bool bInGame );
	virtual void Event_PlayerEnteredGodMode( CBasePlayerPawn *pBasePlayer );
	virtual void Event_PlayerEnteredNoClip( CBasePlayerPawn *pBasePlayer );
	virtual void Event_DecrementPlayerEnteredNoClip( CBasePlayerPawn *pBasePlayer );
	virtual void Event_IncrementCountedStatistic( const Vector &vecAbsOrigin, const char *pchStatisticName, float flIncrementAmount );

	virtual void AppendCustomDataToSaveBuffer( CUtlBuffer &SaveBuffer );
	virtual void LoadCustomDataFromBuffer( CUtlBuffer &LoadBuffer );

	virtual void LoadingEvent_PlayerIDDifferentThanLoadedStats();

	virtual bool LoadFromFile();
	virtual bool SaveToFileNOW( bool bForceSyncWrite = false );
	virtual bool UploadStatsFileNOW();

	virtual bool AutoSave_OnInit();
	virtual bool AutoSave_OnShutdown();
	virtual bool AutoSave_OnMapChange();
	virtual bool AutoSave_OnLevelInit();
	virtual bool AutoSave_OnLevelShutdown();

	virtual bool AutoUpload_OnInit();
	virtual bool AutoUpload_OnShutdown();
	virtual bool AutoUpload_OnMapChange();
	virtual bool AutoUpload_OnLevelInit();
	virtual bool AutoUpload_OnLevelShutdown();

public:
	BasicGameStats_t m_BasicStats;
	bool m_bLogging : 1;
	bool m_bLoggingToFile : 1;
};

// 8 bytes
struct StatsBufferRecord_t
{
	float m_flFrameRate;
	float m_flServerPing;
};

#define STATS_WINDOW_SIZE ( 60 * 10 ) // # of records to hold

//-----------------------------------------------------------------------------
// Static instance, Windows 5064 / Linux 5056 bytes (CUserCmd is 152 / 144). m_tLastUpload is 4
// bytes (only the constructor clears it).
// It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CBaseGameStats_Driver : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CBaseGameStats_Driver();
	virtual ~CBaseGameStats_Driver();

	// Picks the game's stats object (OnInit), sets the stat file names and the logging flags
	virtual bool Init() override;
	// Event_Shutdown, then saves / uploads / sends the stats
	virtual void Shutdown() override;
	// Level init: Event_LevelInit, m_flLevelStartTime, auto save / upload
	GS_EVENT( GameInit );
	// Level shutdown: elapsed time, Event_LevelShutdown, auto save / upload / send
	GS_EVENT( GameDeactivate );
	// Tracks the pause time (m_bGamePaused / m_flPauseStartTime)
	GS_EVENT( ServerPostEntityThink );
	// Event_SaveGame / Event_LoadGame
	GS_EVENT( SaveGame );
	GS_EVENT( RestoreGame );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	StatsBufferRecord_t m_StatsBuffer[ STATS_WINDOW_SIZE ];
	bool m_bBufferFull;
	int m_nWriteIndex;
	float m_flLastRealTime;
	float m_flLastSampleTime;
	float m_flTotalTimeInLevels;
	int m_iNumLevels;
	bool m_bDidVoiceChat;
	CUtlString m_PrevMapName;
	int m_iLoadedVersion;
	char m_szLoadedUserID[ 17 ];
	bool m_bShuttingDown;
	bool m_bInLevel;
	bool m_bFirstLevel;
	int32 m_tLastUpload;
	float m_flLevelStartTime;
	bool m_bStationary;
	float m_flLastMovementTime;
	CUserCmd m_LastUserCmd;
	bool m_bGamePaused;
	float m_flPauseStartTime;
	CGamestatsData *m_pGamestatsData;
};

#endif // GAMESTATS_H
