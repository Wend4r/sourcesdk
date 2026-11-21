//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that turns the func_rotator entities. Factory
//			"CFuncRotatorSystem"; named after func_mover.h, no Valve path names this header.
//
//=============================================================================//

#ifndef FUNC_ROTATOR_H
#define FUNC_ROTATOR_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "tier0/utlvector.h"

class CFuncRotator;

//-----------------------------------------------------------------------------
// Static instance, 48 bytes on both platforms.
//-----------------------------------------------------------------------------
class CFuncRotatorSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Turns every rotator ("Rotators" parallel job when enabled), then lets each one finish
	// its move
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it holds: the registered func_rotator entities
	CUtlVector< CHandle< CFuncRotator > > m_Rotators;
};

#endif // FUNC_ROTATOR_H
