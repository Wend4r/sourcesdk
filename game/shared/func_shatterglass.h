//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that breaks the queued func_shatterglass panes a few per frame.
//
//=============================================================================//

#ifndef FUNC_SHATTERGLASS_H
#define FUNC_SHATTERGLASS_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <entity2/entityidentity.h>
#include <tier0/utlvector.h>

class KeyValues3;

//-----------------------------------------------------------------------------
// Registered as "CShatterGlassGameSystem" (static factory). 64 bytes on both platforms. No source
// has the class; the names are named by what the code does.
//-----------------------------------------------------------------------------
class CShatterGlassGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CShatterGlassGameSystem();

	// Destroys m_pKeyValues
	GS_EVENT( GameShutdown );
	// Passes every pane of m_QueuedPanes to one entity function, empties it and clears m_bGlassSpawned
	GS_EVENT( GamePreShutdown );
	// Zeroes m_nPanesBrokenThisFrame
	GS_EVENT( ServerPreEntityThink );
	// Breaks queued panes while m_bEnabled and m_bGlassSpawned are set, fewer than 150 are queued
	// and fewer than 5 broke this frame
	GS_EVENT( ServerPostEntityThink );
	// Warns once when the global shard count passes 2048 ("Global func_shatterglass shard count has
	// exceeded global max!")
	GS_EVENT( ServerPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// A KeyValues3 (16 bytes) that GameShutdown frees; no writer of it is left in this build
	KeyValues3 *m_pKeyValues;
	// The panes that wait to break; a pane marks itself queued when it is added
	CUtlVector< CEntityHandle > m_QueuedPanes;
	int m_nPanesBrokenThisFrame;
	// Set by the static instance's constructor
	bool m_bEnabled;
	// Set when a func_shatterglass spawns, cleared by GamePreShutdown
	bool m_bGlassSpawned;
};

#endif // FUNC_SHATTERGLASS_H
