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
#include "appframework/gameappsystem.h"

#define GAMECLIENTEXPORTS_INTERFACE_VERSION "GameClientExports001"

abstract_class IGameClientExports : public IAppSystem
{
public:
	virtual bool IsPlayerGameVoiceMuted( int nPlayerSlot ) = 0;
	// Both are empty in this build
	virtual void MutePlayerGameVoice( int nPlayerSlot ) = 0;
	virtual void UnmutePlayerGameVoice( int nPlayerSlot ) = 0;

	// Fire the "gameui_activated" / "gameui_hidden" game events
	virtual void OnGameUIActivated() = 0;
	virtual void OnGameUIHidden() = 0;

	virtual bool unk016() = 0;
	virtual void *unk017() = 0;
	virtual bool unk018() = 0;
};

//-----------------------------------------------------------------------------
// Client implementation of IGameClientExports
//-----------------------------------------------------------------------------
class CGameClientExports : public CGameAppSystem< IGameClientExports >
{
};

#endif // IGAMECLIENTEXPORTS_H
