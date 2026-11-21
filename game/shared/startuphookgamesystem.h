//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Game systems that only exist to run a hook at a given place of the startup order.
//
//=============================================================================//

#ifndef STARTUPHOOKGAMESYSTEM_H
#define STARTUPHOOKGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// A game system with no members and no handlers of its own; HOOK is the type that holds the
// hook, PRIORITY the place in the init order: the reallocating factory of the derived system
// returns it from GetPriority and false from ShouldAutoAdd. The template overrides nothing
// (on Linux it has its abstract table: the IGameSystem slots, the destructor entries empty,
// YouForgot pure; on Windows the table is folded away). The only instantiation in CS2 is
// CStartupHookGameSystem< GameEvent_RegisterInternal, 17408 >.
//-----------------------------------------------------------------------------
template < class HOOK, int PRIORITY >
class CStartupHookGameSystem : public IGameSystem
{
public:
	enum
	{
		STARTUP_HOOK_PRIORITY = PRIORITY
	};
};

// The hook type of the game event registration (no member is known)
struct GameEvent_RegisterInternal;

//-----------------------------------------------------------------------------
// Registered by a reallocating factory with priority 17408 that is not auto-added. 24 bytes on
// both platforms: only the DECLARE_GAME_SYSTEM byte.
//-----------------------------------------------------------------------------
class GameEvent_RegisterHookupGameSystem : public CStartupHookGameSystem< GameEvent_RegisterInternal, 17408 >
{
public:
	DECLARE_GAME_SYSTEM();

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~GameEvent_RegisterHookupGameSystem();
};

#endif // STARTUPHOOKGAMESYSTEM_H
