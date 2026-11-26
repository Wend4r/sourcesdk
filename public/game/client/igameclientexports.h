//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Exports from the client dll to other dlls
//
//===========================================================================//

#ifndef IGAMECLIENTEXPORTS_H
#define IGAMECLIENTEXPORTS_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"

#define GAMECLIENTEXPORTS_INTERFACE_VERSION "GameClientExports001"

abstract_class IGameClientExports : public IAppSystem
{
public:
	virtual bool IsPlayerGameVoiceMuted( int nPlayerSlot ) = 0;
	// Empty in CS2
	virtual void MutePlayerGameVoice( int nPlayerSlot ) = 0;
	virtual void UnmutePlayerGameVoice( int nPlayerSlot ) = 0;

	// Fire the "gameui_activated" / "gameui_hidden" game events
	virtual void OnGameUIActivated() = 0;
	virtual void OnGameUIHidden() = 0;

	// CS2 returns false, 0 and false
	virtual bool GetPlayerIsAliveUserId( int nUserID, bool &bIsAlive ) = 0;
	virtual int GetNumPlayersAliveHumanPlayersOnTeam( int nTeam ) = 0;
	virtual bool ClientWantsBlurEffect() = 0;
};

#endif // IGAMECLIENTEXPORTS_H
