//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Receives the client's answers to point_clientui_dialog dialogs.
//
//=============================================================================//

#ifndef CLIENTUIDIALOGMANAGER_H
#define CLIENTUIDIALOGMANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Registered as "CClientUIDialogManager" (static factory). It has no members of its own and
// adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CClientUIDialogManager : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CClientUIDialogManager();
	virtual ~CClientUIDialogManager();

	// Register / unregister this system's handler of a client message (slots 12 and 13 of an engine
	// interface, with a delegate to this system)
	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // CLIENTUIDIALOGMANAGER_H
