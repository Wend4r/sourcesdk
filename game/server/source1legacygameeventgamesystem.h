//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Sends the Source 1 style game events to the clients that listen for them.
//
//=============================================================================//

#ifndef SOURCE1LEGACYGAMEEVENTGAMESYSTEM_H
#define SOURCE1LEGACYGAMEEVENTGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "igameevents.h"
#include "gameevents.h"
#include "playerslot.h"

#include <tier0/utlsignalslot.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CSource1LegacyGameEventGameSystem" (reallocating factory). 1120 bytes on both
// platforms. CUtlSlot is its second base (it connects
// the engine signals it listens to in GameInit).
//-----------------------------------------------------------------------------
class CSource1LegacyGameEventGameSystem : public CAutoGameSystem, public CUtlSlot
{
public:
	DECLARE_GAME_SYSTEM();

	// The game event listener of one client: forwards the events it gets to its player as a
	// Source1LegacyGameEvent_t message (16 bytes)
	class CServerSideClient_GameEventLegacyProxy : public IGameEventListener2
	{
	public:
		virtual ~CServerSideClient_GameEventLegacyProxy();
		virtual void FireGameEvent( IGameEvent *event ) override;

	public:
		// The player slot (0..63)
		CPlayerSlot m_nPlayerSlot;
	};

	virtual ~CSource1LegacyGameEventGameSystem();

	// Connects the slot to the engine signals; registers the test script chat scanner when the
	// server runs a test script (m_bRegisteredTestScriptChatScanner)
	GS_EVENT( GameInit );
	// Removes every proxy from the game event manager, disconnects the signals and destroys the
	// queued events
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CServerSideClient_GameEventLegacyProxy m_Clients[ 64 ];
	bool m_bRegisteredTestScriptChatScanner;
	// Named by what it holds; the destructor and GameShutdown destroy them, nothing writes them
	CUtlVector< Source1LegacyGameEvent_t > m_QueuedEvents;
};

#endif // SOURCE1LEGACYGAMEEVENTGAMESYSTEM_H
