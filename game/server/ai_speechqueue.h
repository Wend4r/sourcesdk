//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that owns the queue of deferred AI speech responses.
//
//=============================================================================//

#ifndef AI_SPEECHQUEUE_H
#define AI_SPEECHQUEUE_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

// The queue of deferred responses; 80 bytes, allocated by
// GameInit
class CResponseQueue;

//-----------------------------------------------------------------------------
// Registered as "ResponseQueueManager" (static factory). 32 bytes on both platforms.
//-----------------------------------------------------------------------------
class CResponseQueueManager : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CResponseQueueManager();

	// Allocates m_pQueue (64 entries) when there is none
	GS_EVENT( GameInit );
	// Destroys m_pQueue
	GS_EVENT( GameShutdown );
	// Empties m_pQueue
	GS_EVENT( GameDeactivate );
	// Dispatches the responses of m_pQueue that are due
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CResponseQueue *m_pQueue;
};

#endif // AI_SPEECHQUEUE_H
