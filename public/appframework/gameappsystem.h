#ifndef GAMEAPPSYSTEM_H
#define GAMEAPPSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "tier3/tier3.h"

//-----------------------------------------------------------------------------
// Which side of the game a module connects the game interface globals for.
// Every entry of the module's game interface table names the sides it is used on.
//-----------------------------------------------------------------------------
enum GameInterfaceConnectionMode_t
{
	GAME_INTERFACE_CONNECT_SERVER_ONLY = 1,
	GAME_INTERFACE_CONNECT_CLIENT_ONLY = 2,
	GAME_INTERFACE_CONNECT_SERVER_AND_CLIENT = 3,
};

//-----------------------------------------------------------------------------
// Connects the interfaces that only the game modules use (DotaMapUtils, HostUtils,
// PredictionDiffManager, DebugDrawQueueManager, ResponseRulesCache, GameSystem2HostHook,
// SinglePlayerSharedMemory, EntitySubclassUtilsV001 (client only: the server provides it), EconVData) and
// points the side-specific globals (game resource service, string table container)
// at the server or client instance. A module must always use the same mode.
//-----------------------------------------------------------------------------
void ConnectGameInterfaces( CreateInterfaceFn *pFactoryList, int nFactoryCount, GameInterfaceConnectionMode_t eMode );
void DisconnectGameInterfaces();
// Reconnects one interface, or every interface of the mode when pInterfaceName is NULL
void ReconnectGameInterface( CreateInterfaceFn factory, const char *pInterfaceName, GameInterfaceConnectionMode_t eMode );

//-----------------------------------------------------------------------------
// Base of the interfaces a game module (server, client) exports to the engine
//-----------------------------------------------------------------------------
template< class IInterface, int ConVarFlag = 0 >
class CGameAppSystem : public CTier3AppSystem< IInterface, ConVarFlag >
{
	typedef CTier3AppSystem< IInterface, ConVarFlag > BaseClass;

public:
#ifdef CLIENT_DLL
	static constexpr GameInterfaceConnectionMode_t GAME_INTERFACE_CONNECTION_MODE = GAME_INTERFACE_CONNECT_CLIENT_ONLY;
#else
	static constexpr GameInterfaceConnectionMode_t GAME_INTERFACE_CONNECTION_MODE = GAME_INTERFACE_CONNECT_SERVER_ONLY;
#endif

	virtual bool Connect( CreateInterfaceFn factory )
	{
		if ( !BaseClass::Connect( factory ) )
			return false;

		ConnectGameInterfaces( &factory, 1, GAME_INTERFACE_CONNECTION_MODE );
		return true;
	}

	virtual void Disconnect()
	{
		DisconnectGameInterfaces();
		BaseClass::Disconnect();
	}

	// The game systems sit above every engine tier, so they may depend on all of them
	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER5;
	}

	virtual void Reconnect( CreateInterfaceFn factory, const char *pInterfaceName )
	{
		BaseClass::Reconnect( factory, pInterfaceName );
		ReconnectGameInterface( factory, pInterfaceName, GAME_INTERFACE_CONNECTION_MODE );
	}
};

#endif
