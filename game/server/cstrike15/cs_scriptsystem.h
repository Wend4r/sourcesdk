//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Server-wide state of cs_script (v8): the console commands scripts
//          register and the queued post-entity-think callbacks.
//
//=============================================================================//

#ifndef CS_SCRIPTSYSTEM_H
#define CS_SCRIPTSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "engine/iaddonlistchangenotify.h"

#include <tier0/utlhashtable.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlstring.h>

class CCSBaseScript;
// A console command registered by a script (Console.RegisterCheatCommand), 48 bytes with a vtable
class CCSScriptConCommand;

namespace v8
{
	class Function;
	class Value;

	template < class T >
	class Global;
}

//-----------------------------------------------------------------------------
// Registered as "CCSScriptSystem" (static factory). No source names its members; they are named by
// what the code does with them. It adds no virtual methods to IGameSystem; on Linux the
// OnAddonListChanged override also takes slot 66 of the primary table.
//-----------------------------------------------------------------------------
class CCSScriptSystem : public CAutoGameSystem, public IAddonListChangeNotify
{
public:
	DECLARE_GAME_SYSTEM();

	// A callback queued by a script for the next ServerPostEntityThink ("QueuePostEntityThinks")
	struct QueuedPostEntityThink_t
	{
		CCSBaseScript *m_pScript;
		v8::Global< v8::Function > *m_pCallback;
	};

	CCSScriptSystem();
	virtual ~CCSScriptSystem();

	// Installs itself as the engine's addon list change notify; returns true
	virtual bool Init() override;
	// Disposes m_pGlobal
	GS_EVENT( GameShutdown );
	// Calls the queued callbacks (oldest first) and empties the queue
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// Disposes m_pGlobal
	virtual void OnAddonListChanged() override;

public:
	// A heap-allocated v8 global handle; disposed (and freed) on GameShutdown and when the addon list
	// changes
	v8::Global< v8::Value > *m_pGlobal;
	// The commands scripts registered, by name ("ConCommand:<name>" is their global symbol)
	CUtlHashtable< CUtlString, CCSScriptConCommand * > m_ScriptCommands;
	// A ring buffer: m_nQueueHead is the oldest entry, m_nQueueTail the newest, both -1 when empty
	CUtlLeanVector< QueuedPostEntityThink_t > m_QueuedPostEntityThinks;
	int m_nQueueHead;
	int m_nQueueTail;
};

#endif // CS_SCRIPTSYSTEM_H
