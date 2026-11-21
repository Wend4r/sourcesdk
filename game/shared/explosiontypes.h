//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that loads the explosion type vdata
//			(CS2 game/shared/explosiontypes.cpp, factory "ExplosionTypeSystem").
//
//=============================================================================//

#ifndef EXPLOSIONTYPES_H
#define EXPLOSIONTYPES_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Static instance, 24 bytes on both platforms. The vdata it loads lives in a
// separate global, not in the system.
//-----------------------------------------------------------------------------
class CExplosionTypeSystemGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Load the explosion type vdata into its global table
	GS_EVENT( GameInit );
	// Unload it and clear the table's lookup
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // EXPLOSIONTYPES_H
