//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that owns the game particle manager.
//
//=============================================================================//

#ifndef GAME_PARTICLE_MANAGER_H
#define GAME_PARTICLE_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

// Polymorphic, 128 bytes; holds the CUserMsg_ParticleManager message hook
class CGameParticleManager;

//-----------------------------------------------------------------------------
// Static instance (factory "CGameParticleManagerSystem"), 32 bytes on both
// platforms.
//-----------------------------------------------------------------------------
class CGameParticleManagerSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Allocates m_pParticleManager
	GS_EVENT( GameInit );
	// Deletes m_pParticleManager
	GS_EVENT( GameShutdown );
	// Empty on the server
	GS_EVENT( ClientPreRender );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it holds: the particle manager of the running game, NULL outside of it
	CGameParticleManager *m_pParticleManager;
};

#endif // GAME_PARTICLE_MANAGER_H
