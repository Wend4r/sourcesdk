//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that loads the noise stream vdata
//			(CS2 game/shared/noisestreams_vdata.cpp, factory "NoiseStreamSystem").
//
//=============================================================================//

#ifndef NOISESTREAMS_VDATA_H
#define NOISESTREAMS_VDATA_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Static instance, 24 bytes on both platforms. The vdata it loads lives in a
// separate global, not in the system.
//-----------------------------------------------------------------------------
class CNoiseStreamSystemGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Load the noise stream vdata into its global table
	GS_EVENT( GameInit );
	// Unload it and clear the table's lookup
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;
};

#endif // NOISESTREAMS_VDATA_H
