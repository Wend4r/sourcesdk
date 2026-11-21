//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Custom game events between the server's vscript and the clients
//          (CustomGameEventManager script instance).
//
//=============================================================================//

#ifndef CUSTOMGAMEEVENTMANAGER_H
#define CUSTOMGAMEEVENTMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <tier0/utlstringmap.h>
#include <tier0/utlvector.h>

FORWARD_DECLARE_HANDLE( HSCRIPT );

//-----------------------------------------------------------------------------
// Registered as "CustomGameEventManager" (static factory). It adds no virtual methods to
// IGameSystem.
//-----------------------------------------------------------------------------
class CCustomGameEventManager : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// One script listener (24 bytes, operator new)
	struct CCustomGameEventListener
	{
		// The duplicated script callback; released through the script VM when the listener is freed
		HSCRIPT m_hScriptCallback;
		// Script_RegisterListener zeroes it, nothing else in the server touches it
		void *m_pZeroedOnRegister;
		// Returned by Script_RegisterListener (m_nNextListenerID)
		int m_nListenerID;
	};

	// The listeners of one event name (24 bytes)
	struct CCustomGameEventInfo
	{
		CUtlVector< CCustomGameEventListener * > m_Listeners;
	};

	CCustomGameEventManager();
	virtual ~CCustomGameEventManager();

	// Registers the "CustomGameEventManager" script instance and two client message handlers
	GS_EVENT( GameInit );
	// Unregisters the handlers and frees every listener
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	int Script_RegisterListener( const char *pszEventName, HSCRIPT hCallback );

public:
	CUtlStringMap< CCustomGameEventInfo * > m_CustomGameEvents;
	int m_nNextListenerID;
};

#endif // CUSTOMGAMEEVENTMANAGER_H
