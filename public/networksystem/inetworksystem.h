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

	virtual int  CreateSocket( int, int, int, int, int, const char* ) = 0;
	virtual bool OpenSocket( int nSocketIdx, const netadr_t& adr ) = 0;
	virtual bool OpenSocket( int nSocketIdx ) = 0;

	virtual bool IsOpen( int sock ) = 0;

	virtual bool CloseSocket( int sock ) = 0;
	virtual bool ForceReopenSocket( int sock, int ) = 0;

	virtual void SetRemoteStreamChannel( int, int ) = 0;

	virtual void PollSocket( int socket, IConnectionlessPacketHandler *pHandler ) = 0;
	virtual void ProcessSocketMessages( int socket ) = 0;

	virtual INetChannel* CreateNetChannel( int socket, const ns_address* adr, HSteamNetConnection conn, const char* name, int32, int32 ) = 0;
	virtual void         RemoveNetChannel( INetChannel * netchan, bool bDeleteNetChan ) = 0;
	virtual void         RemoveNetChannelByAddress( int socket, const CPeerToPeerAddress & ) = 0;

	virtual void   SetTime( double time ) = 0;
	virtual void   SetTimeScale( float timeScale ) = 0;
	virtual double GetNetTime() const = 0;

	virtual void DescribeSocket( int sock ) = 0;

	virtual bool IsValidSocket( int sock ) = 0;

	virtual void BufferToBufferCompress( char *, unsigned int *, const char *, unsigned int ) = 0;
	virtual void BufferToBufferDecompress( char *, unsigned int *, char const *, unsigned int ) = 0;

	virtual netadr_t& GetPublicAdr() = 0;
	virtual netadr_t& GetLocalAdr() = 0;

	virtual uint16 GetUDPPort( int nClientIdx ) = 0;
	virtual uint16 GetUDPPort( int nClientIdx ) const = 0;

	virtual void ConnectClient( CServerSideClientBase *pClient ) = 0;
	virtual void DisconnectClient( CServerSideClientBase *pClient ) = 0;

	virtual void CloseAllSockets() = 0;

	virtual NetScratchBuffer_t* GetScratchBuffer(void) = 0;
	virtual void                PutScratchBuffer(NetScratchBuffer_t*) = 0;

	virtual class ISteamNetworkingUtils* SteamNetworkingUtils() = 0;

	virtual class ISteamNetworkingSockets* SteamNetworkingSockets() = 0;
	virtual class ISteamNetworkingSockets* SteamGameServerNetworkingSockets() = 0;
	virtual class ISteamNetworkingSockets* SteamGameServerNetworkingSockets_Lib() = 0;

	virtual class ISteamNetworkingMessages* SteamNetworkingMessages_Lib() = 0;

private:
	virtual uint32  unk_46( int nClientIdx ) = 0;
	virtual EResult unk_47( CServerSideClientBase *pClient, ESteamNetConnectionEnd eSteamNetConnectionEnd ) = 0;

public:
	// engine2.dll -> str -> \"Rejecting connection request from %s (reservation cookie 0x%llx), server is reserved with reservation cookie\" v virtual function
	virtual void RejectConnection( HSteamNetConnection hPeer, ENetworkDisconnectionReason eReason, SteamNetConnectionInfo_t *pInfo = nullptr ) = 0;

	virtual void RunCallbacks( bool bGameServerCallbacks, void *pSteamServer, void *pFunc, int nParam = 0 ) = 0;
	virtual void RunFrame( bool bGameServerCallbacks ) = 0;

	virtual void SteamAPIInit() = 0;

private:
	virtual bool unk_52() = 0;
	virtual void unk_53(netadr_t & tAdr, double dbTimeout) = 0;

public:
	virtual ~INetworkSystem() {}
};

DECLARE_TIER2_INTERFACE( INetworkSystem, g_pNetworkSystem );

#endif // INETWORKSYSTEM_H
