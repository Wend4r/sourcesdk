//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that blends the host timescale. Factory "GameTimescale".
//
//=============================================================================//

#ifndef GAME_TIMESCALE_SHARED_H
#define GAME_TIMESCALE_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Static instance, 48 bytes on both platforms.
//-----------------------------------------------------------------------------
class CGameTimescale : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Init, Shutdown, GameShutdown and PostSpawnGroupLoad reset the blend (desired and current
	// timescale 1, blend rate = min blend rate, acceleration 0.05, min blend rate 0.1, blend
	// delta multiplier 3) and set the engine timescale to 1
	virtual bool Init() override;
	virtual void Shutdown() override;
	GS_EVENT( GameShutdown );
	GS_EVENT( PostSpawnGroupLoad );
	// Moves m_flCurrentTimescale toward m_flDesiredTimescale and applies it to the engine
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	float m_flDesiredTimescale;
	float m_flCurrentTimescale;
	float m_flBlendRate;
	float m_flAcceleration;
	float m_flMinBlendRate;
	float m_flBlendDeltaMultiplier;
	// Global real time when ServerPostEntityThink last blended
	float m_flLastUpdateRealtime;
};

#endif // GAME_TIMESCALE_SHARED_H
