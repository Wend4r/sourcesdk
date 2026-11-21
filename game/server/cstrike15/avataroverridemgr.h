//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Applies avatar overrides to players when they finish connecting.
//
//=============================================================================//

#ifndef AVATAROVERRIDEMGR_H
#define AVATAROVERRIDEMGR_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

// CGameEventListener
#include <vscript/ccscript.h>

//-----------------------------------------------------------------------------
// Registered as "AvatarOverrideManager" (static factory). Everything it does is gated by a
// convar; it adds no virtual methods to IGameSystem. On Linux the FireGameEvent override also
// takes slot 66 of the primary table (Itanium), on Windows it is only in the second table.
//-----------------------------------------------------------------------------
class CAvatarOverrideMgr : public CAutoGameSystem, public CGameEventListener
{
public:
	DECLARE_GAME_SYSTEM();

	CAvatarOverrideMgr();
	virtual ~CAvatarOverrideMgr();

	// Listens to "player_connect_full" (sets m_bRegisteredForEvents) when the convar is on
	GS_EVENT( GameInit );
	// Stops listening
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// "player_connect_full": applies the override for the event's "userid"
	virtual void FireGameEvent( IGameEvent *event ) override;
};

#endif // AVATAROVERRIDEMGR_H
