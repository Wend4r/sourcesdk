//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Host-side driver of the game systems of a loop: init, activate,
//          deactivate and shut down every registered game system
//
//=============================================================================//

#ifndef IGAMESYSTEM2HOSTHOOK_H
#define IGAMESYSTEM2HOSTHOOK_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>

class GameSessionConfiguration_t;
class ILoopModePrerequisiteRegistry;
class IGameSystemEventDispatcher;
struct EngineLoopState_t;

abstract_class IGameSystem2HostHook : public IAppSystem
{
public:
	// Keeps pDispatcher until LoopShutdownAllSystems and initialises the game systems
	virtual void LoopInitAllSystems( const GameSessionConfiguration_t &config, ILoopModePrerequisiteRegistry *pRegistry, IGameSystemEventDispatcher *pDispatcher ) = 0;

	// Shuts the game systems down in reverse order and forgets the dispatcher
	virtual void LoopShutdownAllSystems() = 0;

	// state and bBackgroundMap are handed to the GameActivate / GameDeactivate events of the game systems
	virtual void LoopActivateAllSystems( const EngineLoopState_t &state, bool bBackgroundMap ) = 0;
	virtual void LoopDeactivateAllSystems( const EngineLoopState_t &state, bool bBackgroundMap ) = 0;
};

#endif // IGAMESYSTEM2HOSTHOOK_H
