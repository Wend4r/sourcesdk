//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Spawns and respawns the ambient chickens of a map on its nav mesh.
//
//=============================================================================//

#ifndef CS_CHICKENMANAGER_H
#define CS_CHICKENMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "bot/cs_bot_timers.h"

#include <tier0/utlvector.h>

// CGameEventListener
#include <vscript/ccscript.h>

//-----------------------------------------------------------------------------
// Registered as "CCSChickenManager" (static factory). No source names its members; they are named
// by what the code does with them. It adds no virtual methods to IGameSystem. On Linux the
// FireGameEvent override also takes slot 66 of the primary table.
//-----------------------------------------------------------------------------
class CCSChickenManager : public CAutoGameSystem, public CGameEventListener
{
public:
	DECLARE_GAME_SYSTEM();

	CCSChickenManager();
	virtual ~CCSChickenManager();

	// Resets both timers and listens to "round_start", "round_freeze_end" and "round_end"
	virtual bool Init() override;
	// Restarts m_SpawnTimer (random 0..10 s) and tops the chickens up when it elapsed; releases the
	// chickens when m_ReleaseTimer elapsed
	GS_EVENT( ServerPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// "round_start": places the chickens; "round_freeze_end": starts m_ReleaseTimer (5 s);
	// "round_end": stores the "winner"
	virtual void FireGameEvent( IGameEvent *event ) override;

public:
	// Checked every ServerPostSimulate; tops the chicken count up when it elapses
	CountdownTimer m_SpawnTimer;
	// The listener IDs AddListener returned for "round_start", "round_freeze_end" and "round_end";
	// FireGameEvent tells the events apart by them
	int m_nRoundStartEventID;
	int m_nRoundFreezeEndEventID;
	int m_nRoundEndEventID;
	// Nav area IDs a chicken may spawn in: anywhere, near the T spawn, near the CT spawn
	CUtlVector< uint32 > m_SpawnAreas;
	CUtlVector< uint32 > m_TSpawnAreas;
	CUtlVector< uint32 > m_CTSpawnAreas;
	// Started by "round_freeze_end"; when it elapses the chickens are let go
	CountdownTimer m_ReleaseTimer;
	// The "winner" of the last "round_end", -1 after the chickens were placed for the new round
	int m_nLastRoundWinner;
};

#endif // CS_CHICKENMANAGER_H
