//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that logs the precache phase of GameInit.
//
//=============================================================================//

#ifndef PRECACHE_REGISTER_H
#define PRECACHE_REGISTER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Reallocating game system, 24 bytes on both platforms (no
// members besides the DECLARE_GAME_SYSTEM one).
//-----------------------------------------------------------------------------
class CPrecacheRegister : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Logs "GameInit - PreCache - Start" and "GameInit - PreCache - Finish"
	GS_EVENT( GameInit );
	// Empty
	GS_EVENT( GameShutdown );
	// Empty
	GS_EVENT( GameActivate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // PRECACHE_REGISTER_H
