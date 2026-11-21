//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Writes game events to the server log.
//
//=============================================================================//

#ifndef EVENTLOG_H
#define EVENTLOG_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

// CGameEventListener
#include <vscript/ccscript.h>

class CBaseEntity;
class IGameEvent;

//-----------------------------------------------------------------------------
// 40 bytes on both platforms: CGameEventListener (16), IGameSystem (16) and the member
// DECLARE_GAME_SYSTEM() adds. Never created itself;
// CCSEventLog (cs_eventlog.h) is the registered system.
//
// The primary table is the one of CGameEventListener: it holds the destructor,
// FireGameEvent and the six methods the class adds (FormatPlayer and the Print family).
// libserver.so also repeats the IGameSystem overrides there, in declaration order
// (slots 3-6: YouForgot_DECLARE_GAME_SYSTEM_InYourClassDefinition,
// DoesGameSystemReallocate, SetGameSystemGlobalPtrs, GameInit), so they are declared in
// that order.
//-----------------------------------------------------------------------------
class CEventLog : public CGameEventListener, public IGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CEventLog();

	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	// Listens to player_changename, player_activate, player_death, player_team,
	// player_disconnect and player_connect
	GS_EVENT( GameInit );

	// IGameEventListener2: PrintEvent while the log is enabled
	virtual void FireGameEvent( IGameEvent *event ) override;

	// Copies the player description into str (nothing for a null entity or a size below 1)
	virtual void FormatPlayer( CBaseEntity *ent, char *str, int len ) const;

protected:
	// Dispatches by the prefix of the event name: "server_" is ignored, "player_" goes to
	// PrintPlayerEvent, "team_" to PrintTeamEvent, "game_" to PrintGameEvent, the rest to
	// PrintOtherEvent
	virtual bool PrintEvent( IGameEvent *event );
	// Returns false here
	virtual bool PrintGameEvent( IGameEvent *event );
	virtual bool PrintPlayerEvent( IGameEvent *event );
	// Returns false here
	virtual bool PrintTeamEvent( IGameEvent *event );
	// Returns false here
	virtual bool PrintOtherEvent( IGameEvent *event );
};

#endif // EVENTLOG_H
