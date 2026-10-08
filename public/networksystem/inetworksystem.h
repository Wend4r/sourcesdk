//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
//===========================================================================//

#ifndef INETWORKSYSTEM_H
#define INETWORKSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/netadr.h"
#include "appframework/iappsystem.h"
#include "inetchannel.h"
#include "steam/steamnetworkingtypes.h"

class IConnectionlessPacketHandler;

class NetScratchBuffer_t;
class CPeerToPeerAddress;
class CServerSideClientBase;

enum ENSAddressType
{
	kAddressDirect,
	kAddressP2P,
	kAddressProxiedGameServer,
	kAddressProxiedClient,

	kAddressMax
};

struct ns_address; // <tier1/ns_address.h>

// Reversed by @mr.2b. (from Discord)
abstract_class INetworkSystem : public IAppSystem
{
public:
	virtual void InitGameServer() = 0;
	virtual void ShutdownGameServer() = 0;

	virtual int CreateSocket( int nPort, int nDefaultPort, int nSocketType, int nLoopbackId, int nFlags, const char *pszName ) = 0;

	virtual bool OpenSocket( int nSocket ) = 0;

	virtual bool ConnectSocket( int nSocket, const ns_address &adr ) = 0;

	virtual bool IsOpen( int nSocket ) = 0;

	virtual bool CloseSocket( int nSocket ) = 0;

	// Pairs two sockets through a loopback Steam connection pair.
	virtual bool ConnectLoopback( int nSocket1, int nSocket2 ) = 0;

	virtual void SetDefaultUDPPort( int nPort ) = 0;

	virtual void PollSocket( int nSocket, IConnectionlessPacketHandler *pHandler ) = 0;
	virtual void ProcessSocketMessages( int nSocket ) = 0;

	virtual INetChannel *CreateNetChannel( int nSocket, const ns_address *adr, HSteamNetConnection hConnection, const char *pszName, int32, int32, bool bPlayback ) = 0;
	virtual void RemoveNetChannel( INetChannel *pNetChannel, bool bDeleteNetChan ) = 0;
	virtual bool RemoveNetChannelByAddress( int nSocket, const CPeerToPeerAddress &adr ) = 0;

	virtual void SetTime( double flTime ) = 0;
	virtual void SetTimeScale( float flTimeScale ) = 0;
	virtual double GetNetTime() const = 0;

	// Returns the socket name, or "???" for an index out of range.
	virtual const char *DescribeSocket( int nSocket ) = 0;

	virtual bool IsValidSocket( int nSocket ) = 0;

	virtual bool BufferToBufferCompress( char *pDest, unsigned int *pnDestLen, const char *pSource, unsigned int nSourceLen ) = 0;
	virtual bool BufferToBufferDecompress( char *pDest, unsigned int *pnDestLen, const char *pSource, unsigned int nSourceLen ) = 0;

	virtual netadr_t &GetPublicAdr() = 0;
	virtual netadr_t &GetLocalAdr() = 0;

	// Port the socket is bound to, 0 if it is not listening.
	virtual uint16 GetUDPPort( int nSocket ) = 0;

	virtual uint16 GetUDPPortWithFallback( int nSocket ) = 0;

	virtual void ConnectClient( CServerSideClientBase *pClient ) = 0;
	virtual void DisconnectClient( CServerSideClientBase *pClient ) = 0;

	virtual void CloseAllSockets() = 0;

	virtual NetScratchBuffer_t *GetScratchBuffer() = 0;
	virtual void PutScratchBuffer( NetScratchBuffer_t *pBuffer ) = 0;

	virtual class ISteamNetworkingUtils *SteamNetworkingUtils() = 0;

	virtual class ISteamNetworkingSockets *SteamNetworkingSockets() = 0;
	virtual class ISteamNetworkingSockets *SteamGameServerNetworkingSockets() = 0;
	virtual class ISteamNetworkingSockets *SteamGameServerNetworkingSockets_Lib() = 0;

	virtual class ISteamNetworkingMessages *SteamNetworkingMessages_Lib() = 0;

	virtual HSteamNetConnection GetSocketConnection( int nSocket ) = 0;

	virtual ENetworkDisconnectionReason GetDisconnectionReason( const SteamNetConnectionInfo_t *pInfo, ESteamNetworkingConnectionState eOldState ) = 0;

	virtual void RejectConnection( HSteamNetConnection hPeer, ENetworkDisconnectionReason eReason, const char *pszDebug = nullptr ) = 0;

	virtual void RunCallbacks( bool bGameServerCallbacks, void *pCallbackContext, void *pfnCallback ) = 0;
	virtual void RunFrame( bool bGameServerCallbacks ) = 0;

	virtual bool InitSteamNetworking() = 0;

	virtual bool IsNetGraphActive() = 0;

	// nTimeout is added to the current Steam networking timestamp.
	virtual HSteamNetConnection EstablishCacheableSharedNetConnection( const ns_address &adr, SteamNetworkingMicroseconds nTimeout ) = 0;

	virtual ~INetworkSystem() {}
};

DECLARE_TIER2_INTERFACE( INetworkSystem, g_pNetworkSystem );

#endif // INETWORKSYSTEM_H
