//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that runs the bot manager every tick. Factory
//			"BotGameSystem".
//
//			Valve has no header of this name: the system drives the bot manager
//			(TheBots, bot_manager.h next to this file; CS2 builds it from
//			game/shared/cstrike15/bot/bot_manager.cpp), so it presumably lives in
//			bot_manager.cpp. It has a header of its own here so that bot_manager.h
//			stays as the bot manager work left it.
//
//=============================================================================//

#ifndef CSTRIKE15_BOT_GAMESYSTEM_H
#define CSTRIKE15_BOT_GAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

//-----------------------------------------------------------------------------
// The class name comes from the RTTI of the game. Reallocating factory, 24 bytes on both
// platforms: IGameSystem and the member DECLARE_GAME_SYSTEM() adds. It adds no virtual
// methods.
//-----------------------------------------------------------------------------
class CBotGameSystem : public IGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CBotGameSystem();

	// CBotManager::StartFrame of TheBots (slot 8) when there is a bot manager
	GS_EVENT( ServerPreEntityThink );

	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
};

#endif // CSTRIKE15_BOT_GAMESYSTEM_H
