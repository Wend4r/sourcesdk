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
#include "inetchannel.h"
#include "inetmsghandler.h"
#include "matchmaking/imatchasync.h"

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

public:
	// Get engine build number
	virtual int GetEngineBuildNumber() = 0;

	virtual void unk012( void *pInfo ) = 0;

	virtual void unk013( void *pInfo ) = 0;

	virtual void GetServerInfo( void *pServerInfo ) = 0;

	virtual void GetClientInfo( void *pClientInfo ) = 0;

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

	virtual bool Unk_SetNetworkConfig( void *p ) = 0;
};

#define INETSUPPORT_VERSION_STRING "INETSUPPORT_001"


#endif // INETSUPPORT_H
