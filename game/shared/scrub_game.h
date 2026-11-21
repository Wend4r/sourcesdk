//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game side of the scrub system: removes the scrubbed entities.
//
//=============================================================================//

#ifndef SCRUB_GAME_H
#define SCRUB_GAME_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// Listener of the scrub system (RTTI). No virtual destructor, no fields. The method names are
// not in any source: they are named by what the only implementation does.
//-----------------------------------------------------------------------------
abstract_class IScrubSystemEventListener
{
public:
	// CScrubEntityGameSystem collects the entities to scrub, removes them (UTIL_Remove) and logs
	// each one ("%s %s:%s")
	virtual void OnScrubEntities() = 0;
	// Empty in every implementation
	virtual void OnScrubFinished() = 0;
};

// The base the game systems derive from (RTTI); it adds nothing to the interface
class CScrubSystemEventListener : public IScrubSystemEventListener
{
};

//-----------------------------------------------------------------------------
// Registered as "ScrubEntityGameSystem" (static factory). 32 bytes on both platforms.
//-----------------------------------------------------------------------------
class CScrubEntityGameSystem : public CAutoGameSystem, public CScrubSystemEventListener
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CScrubEntityGameSystem();

	// All empty in the server (client events of a shared class)
	GS_EVENT( ClientPreRender );
	GS_EVENT( ClientPostRender );
	GS_EVENT( ServerPostSimulate );
	GS_EVENT( ClientPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IScrubSystemEventListener
	virtual void OnScrubEntities() override;
	virtual void OnScrubFinished() override;

public:
	// Named by what the code does with it: set to false when the static instance is constructed,
	// read nowhere
	bool m_bClearedAtConstruction;
};

#endif // SCRUB_GAME_H
