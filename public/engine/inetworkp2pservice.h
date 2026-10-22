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
#include <steam/steamnetworkingtypes.h>
#include <tier0/bitvec.h>
#include <tier0/convar.h>
#include <tier0/threadtools.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlvector.h>

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

class CNetworkP2PService : public CBaseEngineService< INetworkP2PService >, public CUtlSlot
{
public:
	// Peer listed by the list peers command as "Known Peer State"
	struct KnownPeer_t
	{
		CPeerToPeerAddress m_Address;
		SteamNetworkingIdentity m_Identity;
		// One bit per entry of m_PeerGroupHandlers that knows the peer
		CVarBitVecBase< int, 64 > m_KnownByHandlers;
		// Shown as "(You)"
		bool m_bLocalUser;
		bool m_bEverConnected;
		bool m_bConnected;
		// Net time of the last successful send, negative until then ("Untested Direct Routing")
		double m_flLastSendTime;
		// Net time the peer was added or the last handler stopped knowing it; a peer that no handler knows is removed 120 seconds later
		double m_flLastKnownTime;
	};

	uint8 m_unk060[ 8 ];
	CUtlVector< IPeerGroupHandler * > m_PeerGroupHandlers;
	CUtlVector< KnownPeer_t * > m_KnownPeers;
	CEventDispatcher< CEventIDManager_Default > m_AllPeersEventDispatcher;
	// Set while the P2P messages of the channel are received and dispatched
	bool m_bReceivingMessages;
	CAtomicMutex m_KnownPeersMutex;
	CAtomicMutex m_PeerGroupHandlersMutex;
	// Earliest time a known peer can expire, -1.0 when there are none
	double m_flNextPeerExpireTime;
	CConCommandMemberAccessor< CNetworkP2PService > m_ListPeersCommand;
};

#endif // INETWORKP2PSERVICE_H
