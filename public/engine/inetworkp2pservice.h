//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service for peer-to-peer messages over Steam networking
//
//=============================================================================//

#ifndef INETWORKP2PSERVICE_H
#define INETWORKP2PSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <IEngineService.h>
#include <ns_address.h>

class INetworkMessageInternal;
class CNetMessage;
struct SteamNetworkingMessagesSessionRequest_t;

abstract_class INetworkP2PService : public IEngineService
{
public:
	virtual bool BroadcastP2PNetMessage( INetworkMessageInternal *pNetMessage, const CNetMessage *pData, const CPeerToPeerAddress *pPeers, int nPeers, bool bSkipFlaggedPeers, int nChannel = -1 ) = 0;

	virtual void AddPeerListener( void *pListener ) = 0;
	virtual void RemovePeerListener( void *pListener ) = 0;

	virtual void unk026( INetworkMessageInternal *pNetMessage ) = 0;

	virtual void RefreshPeerListener( void *pListener ) = 0;

	virtual bool IsKnownPeer( const CPeerToPeerAddress &addr ) = 0;

	virtual void *GetNetMessageDispatcher() = 0;

	virtual bool IsKnownSteamIDPeer( uint64 nSteamID ) = 0;

	virtual void OnP2PSessionRequest( SteamNetworkingMessagesSessionRequest_t *pRequest ) = 0;

	// Empty in this build
	virtual void unk032() = 0;
};

#endif // INETWORKP2PSERVICE_H
