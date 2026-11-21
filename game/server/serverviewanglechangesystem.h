//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Resets the server's view angle change counter at game init.
//
//=============================================================================//

#ifndef SERVERVIEWANGLECHANGESYSTEM_H
#define SERVERVIEWANGLECHANGESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Registered as "ServerViewAngleChangeSystem" (static factory). 24 bytes on both platforms: no
// fields besides the DECLARE_GAME_SYSTEM member.
//-----------------------------------------------------------------------------
class CServerViewAngleChangeSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CServerViewAngleChangeSystem();

	// Zeroes the global counter the player pawns use to number their server-side view angle
	// changes
	GS_EVENT( GameInit );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // SERVERVIEWANGLECHANGESYSTEM_H
