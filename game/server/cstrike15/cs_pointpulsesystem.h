//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Passes the round events to the Pulse graphs of point_pulse entities.
//
//=============================================================================//

#ifndef CS_POINTPULSESYSTEM_H
#define CS_POINTPULSESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

// CGameEventListener
#include <vscript/ccscript.h>

//-----------------------------------------------------------------------------
// Registered as "CCSPointPulseSystem" (static factory). It adds no virtual methods to
// IGameSystem. On Linux the FireGameEvent override also takes slot 66 of the primary table.
//-----------------------------------------------------------------------------
class CCSPointPulseSystem : public CAutoGameSystem, public CGameEventListener
{
public:
	DECLARE_GAME_SYSTEM();

	CCSPointPulseSystem();
	virtual ~CCSPointPulseSystem();

	// Listens to "round_start" and "round_end"; returns true
	virtual bool Init() override;

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// "round_start": dispatches the round start ("round_number") to the Pulse graphs;
	// "round_end": dispatches the round end ("winner", "reason")
	virtual void FireGameEvent( IGameEvent *event ) override;

public:
	// Named by what they hold: the listener IDs AddListener returned; FireGameEvent tells the events
	// apart by them
	int m_nRoundEndEventID;
	int m_nRoundStartEventID;
};

#endif // CS_POINTPULSESYSTEM_H
