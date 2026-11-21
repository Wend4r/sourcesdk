//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that installs the game rules and forwards the game
//			system events to them. Factory "GameRulesGameSystem".
//
//			Valve has the class in game/shared/gamerules.cpp; sourcesdk's gamerules.h is
//			generated, so the class lives in this header of its own.
//
//=============================================================================//

#ifndef GAMERULESGAMESYSTEM_H
#define GAMERULESGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Reallocating factory, 24 bytes on both platforms: IGameSystem and the member
// DECLARE_GAME_SYSTEM() adds. The event handlers have the names of their events here (see
// igamesystem.h). Each handler calls the method of the
// same event of g_pGameRules (CGameRulesVTable, gamerules.h) when the game rules exist;
// GameInit and GameShutdown create and delete them.
//-----------------------------------------------------------------------------
class CGameRulesGameSystem : public IGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CGameRulesGameSystem();

	virtual void PostInit() override;
	// Creates the game rules, calls their Init (or RefreshSkillData), then their GameInit;
	// logs "CGameRulesGameSystem::GameInit installed game rules"
	GS_EVENT( GameInit );
	// GameShutdown and Shutdown of the game rules, then deletes them and clears g_pGameRules;
	// logs "CGameRulesGameSystem::GameShutdown uninstalled game rules"
	GS_EVENT( GameShutdown );
	GS_EVENT( GamePostInit );
	// BuildGameSessionManifest of the game rules (the base one precaches the effect
	// names and the "player" entity)
	GS_EVENT( BuildGameSessionManifest );
	// GameActivate, CreateStandardEntities and InitDefaultAIRelationships of the game rules
	GS_EVENT( GameActivate );
	// DestroyStandardEntities and GameDeactivate of the game rules
	GS_EVENT( GameDeactivate );
	GS_EVENT( SpawnGroupPrecache );
	GS_EVENT( PreSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupLoad );
	GS_EVENT( PreSpawnGroupUnload );
	GS_EVENT( PostSpawnGroupUnload );
	GS_EVENT( ActiveSpawnGroupChanged );
	// FrameUpdatePreEntityThink of the game rules
	GS_EVENT( ServerPreEntityThink );
	// Think of the game rules
	GS_EVENT( ServerPostEntityThink );
	// PreClientUpdate of the game rules
	GS_EVENT( ServerPreClientUpdate );
	GS_EVENT( SaveGame );
	GS_EVENT( RestoreGame );
	GS_EVENT( NewLevelPlayerConnect );

	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
};

#endif // GAMERULESGAMESYSTEM_H
