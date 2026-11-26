//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The loop mode of the game and the spawn group entity filter of map loads.
//
//=============================================================================//

#ifndef LOOPMODEGAME_H
#define LOOPMODEGAME_H

#ifdef _WIN32
#pragma once
#endif

#include "gameentitysystem.h"
#include "iloopmode.h"
#include "game/iloopmodegamesharedstate.h"
#include "materialsystem2/renderattributes.h"
#include "mathlib/vector4d.h"
#include "spawngrouptypes.h"
#include "tier0/convar.h"
#include "tier0/jobthread.h"
#include "tier0/smartptr.h"

class INetworkGameServer;

//-----------------------------------------------------------------------------
// Registered as "mapload" (SPAWN_GROUP_ENTITY_FILTER_FALLBACK); a static object. Passes every
// entity whose "classname" is a non-empty string. 8 bytes.
//-----------------------------------------------------------------------------
class CMapLoadEntityFilter : public ISpawnGroupEntityFilter
{
public:
	void FilterSpawnGroupEntities( SpawnGroupHandle_t hSpawnGroup, bool bIsPrecaching, const CUtlVector< const CEntityKeyValues * > *pInKeyValues, CKV3Arena *pArena, CUtlVector< const CEntityKeyValues * > &outKeyValues ) override;
};

//-----------------------------------------------------------------------------
// The loop mode of the game, 1136 bytes on both platforms (CLoopModeFactory< CLoopModeGame >
// allocates it). The second base sits at offset 16, as in the game. On Linux the overrides of
// ILoopModeGameSharedState also take slots 8-15 of the main table and the destructor takes
// slots 16-17; on Windows the destructor is slot 8.
//-----------------------------------------------------------------------------
abstract_class CLoopModeGame : public CLoopModeBase, public ILoopModeGameSharedState
{
public:
	// The states SetGameSystemState moves the game systems through; LoopInit and ReceivedServerInfo
	// step from allocated to post-initialized, the shutdown back to unallocated. Named by the
	// strings the game logs for them
	enum GameSystemState_t : int32
	{
		GAME_SYSTEM_STATE_UNALLOCATED = 0,
		GAME_SYSTEM_STATE_ALLOCATED,
		GAME_SYSTEM_STATE_INITIALIZED,
		GAME_SYSTEM_STATE_POST_INITIALIZED,
	};

	virtual IWorldSession *GetWorldSession() override = 0;
	virtual ClientServerMode_t GetClientServerMode() override = 0;
	virtual void ReceivedServerInfo( const GameSessionConfiguration_t &config, ILoopModePrerequisiteRegistry *pRegistry ) override = 0;
	virtual bool IsInHeadlessMode() override = 0;
	virtual void SetFadeColor( Vector4D vColor ) override = 0;
	virtual bool IsBackgroundMap() override = 0;
	virtual bool IsLevelTransition() override = 0;
	virtual void GetLevelNames( CUtlString *pPreviousLevel, CUtlString *pMapName ) override = 0;

	virtual ~CLoopModeGame() = 0;

public:
	// No data (1 byte)
	CEventMapEmptyRegistrar< CEventIDManager_Default > m_EventMapRegistrar;

	// The "groups" command (status of every spawn group) and its handler
	CON_COMMAND_MEMBER_F( CLoopModeGame, "groups", OnPrintSpawnGroupStatus, "Show status of all spawn groups.", FCVAR_CHEAT );

	// Released (with the reason) when the loop mode changes its world session or goes
	ISource2WorldSession *m_pWorldSession;
	ClientServerMode_t m_Mode;
	bool m_bInMainMenu;
	bool m_bIsInHeadlessMode;
	// Set by OnLoopActivate, cleared by LoopInit and LoopShutdown
	bool m_bLoopSuccessfullyActivated;
	bool m_bBackgroundMap;
	bool m_bIsLevelTransition;
	// Cleared by the constructor; nothing else reads or writes it
	bool m_bReserved;
	// Named by what it holds: set when the asynchronous post-tick work of the server begins; nothing
	// clears it
	bool m_bAsyncPostTickWorkStarted;

	// Named by what it holds: the thread pool job of the last "Begin Async Server Work"
	CSmartPtr< CThreadedJob, CRefCountAccessor > m_pAsyncPostTickWorkJob;

	CUtlString m_sPreviousLevel;
	CUtlString m_sMapName;

	CRenderAttributes m_pViewGameAttrs[ 1 ];

	SpawnGroupHandle_t m_hStartingActiveSpawnGroup;
	CResourceManifestPrerequisite *m_pClientWorldSessionManifest;
	Vector4D m_vFadeColor;

	// Named by what they hold: two objects allocated when the asynchronous post-tick work begins
	// (each time, over the old pointers) and deleted by the destructor (the first counts itself in a global for as long as it
	// lives; the second has no data)
	void *m_pAsyncPostTickWorkScope;
	void *m_pAsyncPostTickWorkToken;

	// Taken from the engine when the loop activates, released when it deactivates
	INetworkGameServer *m_pGameInstance;
	GameSystemState_t m_nGameSystemState;
};

#endif // LOOPMODEGAME_H
