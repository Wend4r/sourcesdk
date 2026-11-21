//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: CS game stats: per-player and per-team stat collections and the
//          Steamworks stat records of the match.
//
//=============================================================================//

#ifndef CS_GAMESTATS_H
#define CS_GAMESTATS_H
#ifdef _WIN32
#pragma once
#endif

#include "gamestats.h"
#include "igamestattracker.h"

#include <tier0/utldict.h>
#include <tier0/utlvector.h>
#include <vscript/ccscript.h>

// The stat ids (cs_gamestats_shared.h); CS2 has 302 of them
enum CSStatType_t : int;

#define CS_GAMESTATS_STAT_COUNT 302
#define CS_GAMESTATS_MAX_PLAYERS 64

// 1208 bytes
struct StatsCollection_t
{
	int m_iValue[ CS_GAMESTATS_STAT_COUNT ];
};

// 768 bytes
struct KillStats_t
{
	int iNumKilled[ CS_GAMESTATS_MAX_PLAYERS ];
	int iNumKilledBy[ CS_GAMESTATS_MAX_PLAYERS ];
	int iNumKilledByUnanswered[ CS_GAMESTATS_MAX_PLAYERS ];
};

// 4392 bytes
struct PlayerStats_t
{
	StatsCollection_t statsDelta;
	StatsCollection_t statsCurrentRound;
	StatsCollection_t statsCurrentMatch;
	KillStats_t statsKills;
};

// The Steamworks stat records CCSGameStats keeps until it submits them (allocated with new; sizes
// 0x50, 0x28, 0x98, 0x20 and two more). Their layouts are not declared here.
struct CSGameStatsRecord0_t;
struct CSGameStatsRecord1_t;
struct CSGameStatsRecord2_t;
struct CSGameStatsRecord3_t;
struct CSGameStatsRecord4_t;
struct CSGameStatsRecord5_t;

//-----------------------------------------------------------------------------
// Static instance, 283864 bytes on both platforms. CAutoGameSystem and IGameStatTracker are
// bases. After m_PropStatTable come six vectors of record pointers, named by position.
// On Linux the overrides of the secondary bases and the destructor repeat in the main table
// (slots 54-63, in declaration order); on Windows the main table is CBaseGameStats' 54 slots.
//-----------------------------------------------------------------------------
class CCSGameStats : public CBaseGameStats, public CGameEventListener, public CAutoGameSystem, public IGameStatTracker
{
public:
	DECLARE_GAME_SYSTEM();

	// Forward to the game system factory (declared in this order: Linux slots 55 and 56)
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	CCSGameStats();
	virtual ~CCSGameStats();

	virtual bool Init() override;
	// Disseminates the stats to the players (m_fDisseminationTimerLow / High)
	GS_EVENT( ServerPreClientUpdate );

	virtual void SubmitGameStats( KeyValues *pKV ) override;
	virtual StatContainerList_t *GetStatContainerList() override;

	virtual void FireGameEvent( IGameEvent *event ) override;

	// CBaseGameStats
	virtual void Event_LevelInit() override;
	virtual void Event_LevelShutdown( float flElapsed ) override;
	// Empty in CS2
	virtual void Clear() override;
	virtual void Event_PlayerKilled( CBasePlayerPawn *pPlayer, const CTakeDamageInfo &info ) override;
	virtual void Event_PlayerConnected( CBasePlayerPawn *pPlayer ) override;
	virtual void Event_PlayerDisconnected( CBasePlayerPawn *pPlayer ) override;
	virtual void Event_PlayerDamage( CBasePlayerPawn *pBasePlayer, const CTakeDamageInfo &info ) override;
	virtual void Event_PlayerKilledOther( CBasePlayerPawn *pAttacker, CBaseEntity *pVictim, const CTakeDamageInfo &info ) override;

public:
	PlayerStats_t m_aPlayerStats[ CS_GAMESTATS_MAX_PLAYERS ];
	StatsCollection_t m_aTeamStats[ 2 ];

	float m_fDisseminationTimerLow;
	float m_fDisseminationTimerHigh;

	int m_numberOfRoundsForDirectAverages;
	int m_numberOfTerroristEntriesForDirectAverages;
	int m_numberOfCounterTerroristEntriesForDirectAverages;

	// Indexed by int in CS2 (32-byte tree nodes)
	CUtlDict< CSStatType_t, int > m_PropStatTable;

	CUtlVector< CSGameStatsRecord0_t * > m_StatRecords0;
	CUtlVector< CSGameStatsRecord1_t * > m_StatRecords1;
	CUtlVector< CSGameStatsRecord2_t * > m_StatRecords2;
	CUtlVector< CSGameStatsRecord3_t * > m_StatRecords3;
	CUtlVector< CSGameStatsRecord4_t * > m_StatRecords4;
	CUtlVector< CSGameStatsRecord5_t * > m_StatRecords5;
};

#endif // CS_GAMESTATS_H
