//===== Copyright (c) Valve Corporation, All rights reserved. ======//
//
// Purpose: The game configuration app system shared by the client and the server
//
//==================================================================//

#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#pragma once

#include "tier4/tier4.h"
#include "igameconfiguration.h"

//-----------------------------------------------------------------------------
// Base of the game configuration of every game (CCSGameConfiguration here, a static object that
// adds no member). 64 bytes on both platforms.
//
// It overrides the ISource2ServerConfig methods: on Linux each of them also takes a slot of
// the main table (after the IGameConfiguration methods), on Windows they stay in the
// ISource2ServerConfig table only. ExecuteURL comes after them, so it is the last slot of the
// main table on both platforms.
//-----------------------------------------------------------------------------
abstract_class CGameConfiguration : public CTier4AppSystem< IGameConfiguration >
{
public:
	// ISource2ServerConfig
	virtual const char *GetGameDescription( void ) override;
	virtual int GetNetworkVersion( void ) override;
	virtual void GetPlayerLimits( int &minplayers, int &maxplayers, int &defaultMaxPlayers, bool &bIsMultiplayer ) const override;
	// Overrides the ISource2ClientConfig method of the same name too, so it has no slot of its own
	virtual int GetMaxSplitscreenPlayers( void ) override;
	virtual int GetMaxHumanPlayers() override;
	virtual bool AllowPlayerToTakeOverBots() override;
	virtual void OnClientFullyConnect( CEntityIndex nEntityIndex ) override;
	virtual void GetHostStateLoopModeInfo( HostStateLoopModeType_t type, CUtlString &loopModeName, KeyValues **ppLoopModeOptions ) override;
	virtual bool AllowDedicatedServers( EUniverse universe ) const override;
	virtual void GetConVarPrefixesToResetToDefaults( CUtlString &sSemicolonDelimitedPrefixList ) const override;
	virtual bool AllowSaveRestore() override;
	virtual bool ShouldWriteSerializedEntities() override;
	virtual bool IsCommandQueueEnabled() override;
	virtual float GetCommandQueueDilationPercentage() override;
	virtual bool ShouldDedicatedMapCommandChangeLevel() override;
	virtual bool ShouldAllocateNewClientSlots() override;
	virtual bool UseSixtyFourTickInterval() override;
	virtual bool RegisterGameEvents( CreateInterfaceFn factory ) override;

	// Gets the key/value pairs of IGameConfiguration::ParseURLQuery; empty in CS2
	virtual void ExecuteURL( int nArgs, const void *pArgs );

public:
	// A copy of the key values of the game session configuration that SetWorldSession received
	KeyValues *m_pCurrentGameSessionConfigKeyValues;
	// Named by what it holds: "is_tool_controlled_session" of those key values, kept only while the
	// tools allow it
	bool m_bIsToolControlledSession;
};

#endif // GAMECONFIGURATION_H
