//===== Copyright c 1996-2009, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef INETSUPPORT_H
#define INETSUPPORT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/interface.h"
#include "keyvalues.h"
#include "bitbuf.h"
#include "networksystem/inetchannel.h"
#include "inetmsghandler.h"
#include "matchmaking/imatchasync.h"
#include "tier0/netadr.h"

class ISteamNetworkingUtils;

abstract_class INetSupport : public IAppSystem
{
public:
	enum NetworkConsts_t
	{
		NC_MAX_ROUTABLE_PAYLOAD = 1200,
	};

	enum NetworkSocket_t
	{
		NS_SOCK_CLIENT = 0,	// client socket
		NS_SOCK_SERVER,		// server socket
#ifdef _X360
		NS_SOCK_SYSTEMLINK,		// X360 system link
		NS_SOCK_LOBBY,			// X360 matchmaking lobby
		NS_SOCK_TEAMLINK,		// X360 matchmaking inter-team link
#endif
	};

	enum SteamP2PChannelId_t
	{
		// see top of net_steamsocketmgr.cpp for why we need seperate channels for client & server
		SP2PC_RECV_CLIENT = 0,
		SP2PC_RECV_SERVER,
		SP2PC_LOBBY
	};

	struct BaseServerInfo_t
	{
		uint64 m_serverSteamID;
		uint64 m_nTVSecretCode;
		int m_nRelaySlotsMax;
		int m_nRelaysConnected;
		int m_nRelayClientsConnected;
		uint16 m_nPort;
		uint16 m_nHLTVPort;
		bool m_bActive : 1;
		bool m_bIsTVRelay : 1;
		bool m_bDedicated : 1;
		bool m_bInMainMenuBkgnd : 1;
	};

	struct ExtendedServerInfo_t
	{
		netadr_t m_netAdr;
		netadr_t m_netAdrOnline;
		const char *m_szServerName;
		const char *m_szMapName;
		int m_numMaxHumanPlayers;
		int m_numHumanPlayers;
		bool m_bLobbyExclusive : 1;
		bool m_bGroupExclusive : 1;
		// ISteamNetworkingSockets::GetHostedDedicatedServerPort() of the game server sockets, 0 without them
		uint16 m_nHostedDedicatedServerPort;
	};

	struct ServerInfo_t : public BaseServerInfo_t, public ExtendedServerInfo_t
	{
	};

	struct ClientInfo_t
	{
		INetChannel *m_pNetChannel;
		SignonState_t m_nSignonState;
		int m_nSocket;
		int m_numHumanPlayers;
	};

public:
	// Get engine build number
	virtual int GetEngineBuildNumber() = 0;

	virtual void GetBaseServerInfo( BaseServerInfo_t *pBaseServerInfo ) = 0;

	virtual void GetExtendedServerInfo( ExtendedServerInfo_t *pExtendedServerInfo ) = 0;

	// Fills the base part, then the extended part
	virtual void GetServerInfo( ServerInfo_t *pServerInfo ) = 0;

	virtual void GetClientInfo( ClientInfo_t *pClientInfo ) = 0;

	// Update a local server reservation
	virtual void UpdateServerReservation( uint64 uiReservation ) = 0;

	// Update a client reservation before connecting to a server
	virtual void UpdateClientReservation( uint64 uiReservation, uint64 uiMachineIdHost ) = 0;

	virtual void ReserveServer(
		const ns_address &netAdrPublic, const ns_address &netAdrPrivate,
		uint64 nServerReservationCookie, KeyValues *pKVGameSettings,
		IMatchAsyncOperationCallback *pCallback, IMatchAsyncOperation **ppAsyncOperation ) = 0;

	// Check server reservation cookie matches cookie held by client
	virtual bool CheckServerReservation(
		const ns_address &netAdrPublic, uint64 nServerReservationCookie, uint32 uiReservationStage,
		IMatchAsyncOperationCallback *pCallback, IMatchAsyncOperation **ppAsyncOperation ) = 0;

	// Empty in the engine implementation.
	virtual bool ServerPing( const ns_address &netAdrPublic,
		IMatchAsyncOperationCallback *pCallback, IMatchAsyncOperation **ppAsyncOperation ) = 0;

	virtual bool SetNetworkConfig( const void *pData, size_t nDataSize ) = 0;
};

#define INETSUPPORT_VERSION_STRING "INETSUPPORT_001"

class CNetSupportImpl : public CBaseAppSystem< INetSupport >
{
};


#endif // INETSUPPORT_H
