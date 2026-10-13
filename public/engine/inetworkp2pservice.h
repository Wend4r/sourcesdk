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
#include <steam/steamclientpublic.h>

class INetworkMessageInternal;
class CNetMessage;
class IPeerGroupHandler;
struct SteamNetworkingMessagesSessionRequest_t;

abstract_class INetworkP2PService : public IEngineService
{
public:
	virtual bool BroadcastP2PNetMessage( INetworkMessageInternal *pNetMessage, const CNetMessage *pData, const CPeerToPeerAddress *pPeers, int nPeers, bool bSkipFlaggedPeers, int nChannel = -1 ) = 0;

	// Handlers are tracked per peer with one bit each
	virtual void RegisterPeerGroupHandler( IPeerGroupHandler *pHandler ) = 0;
	virtual void UnregisterPeerGroupHandler( IPeerGroupHandler *pHandler ) = 0;

	// Empty in this build
	virtual void RegisterP2PNetMessage( INetworkMessageInternal *pNetMessage ) = 0;

	virtual void PeerGroupChanged( IPeerGroupHandler *pHandler ) = 0;

	virtual bool IsKnownPeer( const CPeerToPeerAddress &addr ) = 0;

	virtual CEventDispatcher< CEventIDManager_Default > *GetAllPeersEventDispatcher() = 0;

	// Checked before a session request is accepted
	virtual bool SteamIDAllowedToP2PConnect( CSteamID steamID ) = 0;

	virtual void OnP2PSessionRequest( SteamNetworkingMessagesSessionRequest_t *pRequest ) = 0;

	// Empty in this build
	virtual void unk032() = 0;
};

#endif // INETWORKP2PSERVICE_H
