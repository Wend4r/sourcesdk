//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that loads the sound events of the session.
//
//=============================================================================//

#ifndef SOUNDEMITTERSYSTEM_H
#define SOUNDEMITTERSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Registered as "SoundEmitterSystem" (static factory, one global object). 24 bytes on both
// platforms: only the DECLARE_GAME_SYSTEM byte; there is no TraceEmitSound virtual.
//-----------------------------------------------------------------------------
class CSoundEmitterSystem : public IGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Returns true
	virtual bool Init() override;

	// Hands the sound event manifest of the session to the sound emitter system
	GS_EVENT( GameInit );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~CSoundEmitterSystem();
};

#endif // SOUNDEMITTERSYSTEM_H
