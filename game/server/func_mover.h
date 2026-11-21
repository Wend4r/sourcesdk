//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that moves the func_mover entities
//			Factory "CFuncMoverSystem".
//
//=============================================================================//

#ifndef FUNC_MOVER_H
#define FUNC_MOVER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "tier0/utlvector.h"

class CFuncMover;

//-----------------------------------------------------------------------------
// Static instance, 72 bytes on both platforms. Every func_mover adds its handle
// to both vectors when it activates and removes it on deletion. The field names
// are named by what the code does with them.
//-----------------------------------------------------------------------------
class CFuncMoverSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Sorts m_SortedMovers and moves the movers (in parallel when func_mover_parallel is set)
	GS_EVENT( ServerPreEntityThink );
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// The func_movers in the order they registered ("func_mover count: %i")
	CUtlVector< CHandle< CFuncMover > > m_Movers;
	// The same movers, sorted by ServerPreEntityThink by a per-mover order value before it
	// moves them
	CUtlVector< CHandle< CFuncMover > > m_SortedMovers;
};

#endif // FUNC_MOVER_H
