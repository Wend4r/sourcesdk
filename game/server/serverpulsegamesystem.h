//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The server's Pulse game system: drives the server clock and keeps
//          the game blackboards.
//
//=============================================================================//

#ifndef SERVERPULSEGAMESYSTEM_H
#define SERVERPULSEGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "basepulsegamesystem.h"
#include "pulse/pulsesymbol.h"

#include <tier0/utlhashtable.h>

//-----------------------------------------------------------------------------
// Reallocating factory, 192 bytes on both platforms. Its m_YouForgot sits right after the base
// (Linux 0x98; Windows 0x98 too: MSVC rounds the base up to 8 after its 12-byte mutex). It adds no
// virtual methods; on Linux the GetSaveStateMemberName override also takes slot 73.
//-----------------------------------------------------------------------------
class CServerPulseGameSystem : public CBasePulseGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CServerPulseGameSystem();
	virtual ~CServerPulseGameSystem();

	// The base's GameInit
	GS_EVENT( GameInit );
	// The base's GameShutdown, then empties m_GameBlackboards
	GS_EVENT( GameShutdown );
	// Runs the "ServerCurTime" clock: delivers the queued outputs and draws the DebugWorldText overlays
	GS_EVENT( ServerPostEntityThink );
	GS_EVENT( RestoreGame );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Returns "server_game_system"
	virtual CKV3MemberName GetSaveStateMemberName() override;

public:
	// The entities that hold a game blackboard graph (CPulseGraphInstance_GameBlackboard), by name
	CUtlHashtable< PulseSymbol_t, CEntityHandle > m_GameBlackboards;
};

#endif // SERVERPULSEGAMESYSTEM_H
