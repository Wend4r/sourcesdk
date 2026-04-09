#ifndef SERVERSIDECLIENT_H
#define SERVERSIDECLIENT_H

#if COMPILER_MSVC
#pragma once
#endif

#include <inetchannel.h>
#include <playerslot.h>
#include <playeruserid.h>

#include <entity2/entityidentity.h>
#include <steam/steamclientpublic.h>
#include <engine/clientframe.h>
#include <tier0/annotations.h>
#include <tier0/circularbuffer.h>
#include <tier0/utlstring.h>
#include <tier0/utlsignalslot.h>
#include <tier1/ns_address.h>
#include <networksystem/inetworksystem.h>

#include <netmessages.h>
#include <utldict.h>

class CHLTVServer;
class INetMessage;
class CNetworkGameServerBase;
class CNetworkGameServer;

class CMsg_CVars;
class CNETMsg_StringCmd_t;
class CNETMsg_Tick_t;
class CNETMsg_SpawnGroup_LoadCompleted_t;
class CCLCMsg_ClientInfo_t;
class CCLCMsg_BaselineAck_t;
class CCLCMsg_LoadingProgress_t;
class CCLCMsg_SplitPlayerConnect_t;
class CCLCMsg_SplitPlayerDisconnect_t;
class CCLCMsg_CmdKeyValues_t;
class CCLCMsg_Move_t;
class CCLCMsg_VoiceData_t;
class CCLCMsg_FileCRCCheck_t;
class CCLCMsg_RespondCvarValue_t;
class NetMessagePacketStart_t;
class NetMessagePacketEnd_t;
class NetMessageConnectionClosed_t;
class NetMessageConnectionCrashed_t;
class NetMessageSplitscreenUserChanged_t;

struct HltvReplayStats_t {
	enum FailEnum_t {
		FAILURE_ALREADY_IN_REPLAY,
		FAILURE_TOO_FREQUENT,
		FAILURE_NO_FRAME,
		FAILURE_NO_FRAME2,
		FAILURE_CANNOT_MATCH_DELAY,
		FAILURE_FRAME_NOT_READY,
		NUM_FAILURES
	};

	uint nClients;
	uint nStartRequests;
	uint nSuccessfulStarts;
	uint nStopRequests;
	uint nAbortStopRequests;
	uint nUserCancels;
	uint nFullReplays;
	uint nNetAbortReplays;
	uint nFailedReplays[NUM_FAILURES];
}; // sizeof 56
COMPILE_TIME_ASSERT(sizeof(HltvReplayStats_t) == 56);

struct Spike_t {
public:
	CUtlString m_szDesc;
	int m_nBits;
};
COMPILE_TIME_ASSERT(sizeof(Spike_t) == 16);

class CNetworkStatTrace {
public:
	CUtlVector<Spike_t> m_Records;
	int m_nMinWarningBytes;
	int m_nStartBit;
	int m_nCurBit;
};
COMPILE_TIME_ASSERT(sizeof(CNetworkStatTrace) == 40);

class CServerSideClientBase : public CUtlSlot, public INetworkChannelNotify, public INetworkMessageProcessingPreFilter
{
public:
	virtual ~CServerSideClientBase() = 0;

public:
	CPlayerSlot              GetPlayerSlot() const { return m_nClientSlot; }
	CPlayerUserId            GetUserID() const { return m_UserID; }
	CEntityIndex             GetEntityIndex() const { return m_nEntityIndex; }
	CSteamID                 GetClientSteamID() const { return m_SteamID; }
	const char              *GetClientName() const { return m_Name; }
	INetChannel             *GetNetChannel() const { return m_NetChannel; }
	const netadr_t          *GetRemoteAddress() const { return m_nAddr.Get<netadr_t>(); }
	CNetworkGameServerBase  *GetServer() const { return m_Server; }

	virtual void             Connect( int socket, const char* pszName, int nUserID, INetChannel* pNetChannel, uint8 nConnectionTypeFlags, uint32 uChallengeNumber ) = 0; // bool bFakePlayer = !nConnectionTypeFlags || (nConnectionTypeFlags & 8) != 0;
	virtual void             Inactivate( const char *pszAddons ) = 0;
	virtual void             Reactivate( CPlayerSlot nSlot ) = 0;
	virtual void             SetServer( CNetworkGameServer *pNetServer ) = 0;
	virtual void             Reconnect() = 0;
	virtual void             Disconnect( ENetworkDisconnectionReason reason, const char *pszInternalReason ) = 0;
	virtual bool             CheckConnect() = 0;
	virtual void             Create( CPlayerSlot &nSlot, CSteamID nSteamID, const char *pszName ) = 0;
	virtual void             SetRate( int nRate ) = 0;
	virtual void             SetUpdateRate( float fUpdateRate ) = 0;
	virtual int              GetRate() = 0;

	virtual void             Clear() = 0;

	virtual bool             ExecuteStringCommand( const CNETMsg_StringCmd_t& msg ) = 0; // "false" trigger an anti spam counter to kick a client.
	virtual bool             SendNetMessage( const CNetMessage *pData, NetChannelBufType_t bufType = BUF_DEFAULT ) = 0;

	// "Client %d(%s) tried to send a RebroadcastSourceId msg.\n"
	virtual bool             FilterMessage( const CNetMessage *pData, INetChannel *pChannel ) = 0; // On Windows, this function is in a separate virtual table

public:
	virtual void             ClientPrintf( PRINTF_FORMAT_STRING const char*, ...) = 0;

	bool                     IsConnected() const { return m_nSignonState >= SIGNONSTATE_CONNECTED; }
	bool                     IsInGame() const { return m_nSignonState == SIGNONSTATE_FULL; }
	bool                     IsSpawned() const { return m_nSignonState >= SIGNONSTATE_NEW; }
	bool                     IsActive() const { return m_nSignonState == SIGNONSTATE_FULL; }
	virtual bool             IsFakeClient() const { return m_bFakePlayer; }
	bool                     IsHLTV() const { return m_bIsHLTV; }
	virtual bool             IsHumanPlayer() const { return false; }

	// Is an actual human player or splitscreen player (not a bot and not a HLTV slot)
	virtual bool             IsHearingClient( CPlayerSlot nSlot ) const { return false; }
	virtual bool             IsProximityHearingClient() const = 0;
	virtual bool             IsLowViolenceClient() const { return m_bLowViolence; }

	virtual bool             IsSplitScreenUser() const { return m_bSplitScreenUser; }

public: // Message Handlers
	virtual bool             ProcessTick( const CNETMsg_Tick_t& msg ) = 0;
	virtual bool             ProcessStringCmd( const CNETMsg_StringCmd_t& msg ) = 0;

public:
	virtual bool             ApplyConVars( const CMsg_CVars &list ) = 0;

	// iterates KV pairs from client, calls KeyValues::SetString on userinfo
	virtual bool             ProcessSetConVar( const CMsg_CVars &list ) = 0;

public:
	virtual bool             ProcessSpawnGroup_LoadCompleted( const CNETMsg_SpawnGroup_LoadCompleted_t &msg ) = 0;
	virtual bool             ProcessClientInfo( const CCLCMsg_ClientInfo_t &msg ) = 0;
	virtual bool             ProcessBaselineAck( const CCLCMsg_BaselineAck_t &msg ) = 0;
	virtual bool             ProcessLoadingProgress( const CCLCMsg_LoadingProgress_t &msg ) = 0;
	virtual bool             ProcessSplitPlayerConnect( const CCLCMsg_SplitPlayerConnect_t &msg ) = 0;
	virtual bool             ProcessSplitPlayerDisconnect( const CCLCMsg_SplitPlayerDisconnect_t &msg ) = 0;
	virtual bool             ProcessCmdKeyValues( const CCLCMsg_CmdKeyValues_t &msg ) = 0;

	virtual bool             SendRconServerDetails() = 0;
	virtual bool             OnSignonStateChanged() { return true; }

public:
	virtual bool             ProcessMove( const CCLCMsg_Move_t &msg ) = 0;
	virtual bool             ProcessVoiceData( const CCLCMsg_VoiceData_t &msg ) = 0;
	virtual bool             ProcessRespondCvarValue( const CCLCMsg_RespondCvarValue_t &msg ) = 0;

	virtual bool             ProcessPacketStart( const NetMessagePacketStart_t &msg ) = 0;
	virtual bool             ProcessPacketEnd( const NetMessagePacketEnd_t &msg ) = 0;
	virtual bool             ProcessConnectionClosed( const NetMessageConnectionClosed_t &msg ) = 0;
	virtual bool             ProcessConnectionCrashed( const NetMessageConnectionCrashed_t &msg ) = 0;

public:
	virtual bool             ProcessChangeSplitscreenUser( const NetMessageSplitscreenUserChanged_t &msg ) = 0;

	virtual bool             UnusedVirtual45() { return false; }
	virtual bool             UnusedVirtual46() { return false; }
	virtual bool             OnDisconnectForwarding() { return true; }

public:
	virtual void             ConnectionStart( INetChannel *pNetChannel ) = 0;

	virtual void             BuildServerInfoMessage() = 0;
	virtual void             PerformPrespawn() = 0;

public:
	virtual void             ExecuteDelayedCall( void * ) = 0;

	virtual bool             UpdateAcknowledgedFramecount( int tick ) = 0;
	void                     ForceFullUpdate()
	{
		// For some reason, it doesn't work.
		// UpdateAcknowledgedFramecount(-1);
		m_nDeltaTick = -1;
	}

	virtual bool             ShouldSendMessages() = 0;
	virtual void             UpdateSendState() = 0;

	virtual const CMsgPlayerInfo &GetPlayerInfo() const { return m_playerInfo; }

	virtual void             UpdateUserSettings() = 0;
	virtual void             ResetUserSettings() = 0;

	virtual void             SendSnapshot() = 0;

public:
	virtual void             SendSignonData() = 0;
	virtual void             SpawnPlayer() = 0;
	virtual void             ActivatePlayer() = 0;

	virtual void             SetName( const char *name ) = 0;
	virtual void             SetUserCVar( const char *cvar, const char *value ) = 0;

	SignonState_t            GetSignonState() const { return m_nSignonState; }

	virtual void             FreeBaselines() = 0;

	bool                     IsFullyAuthenticated( void ) { return m_bFullyAuthenticated; }
	void                     SetFullyAuthenticated( void ) { m_bFullyAuthenticated = true; }

	virtual CServerSideClientBase *GetSplitScreenOwner() { return m_pAttachedTo; }

	virtual int              GetNumPlayers() = 0;

	virtual void             ShouldReceiveStringTableUserData() = 0;

	virtual void             FillServerInfo( void *pInfo ) = 0;
	virtual bool             StartHltvReplay() { return false; }
	virtual void             StopHltvReplay() {}

public:
	virtual int              GetHltvLastSendTick() = 0;

	virtual bool             CanStartHltvReplay() { return false; }
	virtual void             ResetHltvReplayRequestTime() {}
	virtual const char      *GetHltvReplayStats() { return ""; }

public:
	virtual void             Await() = 0;

	virtual void             MarkToKick() = 0;
	virtual void             UnmarkToKick() = 0;

	virtual bool             ProcessSignonStateMsg( int state ) = 0;
	virtual void             PerformDisconnection( ENetworkDisconnectionReason reason ) = 0;

public:
	CUtlString m_UserIDString;
	CUtlString m_Name;
	CPlayerSlot m_nClientSlot;
	CEntityIndex m_nEntityIndex;
	CNetworkGameServerBase* m_Server;
	INetChannel* m_NetChannel;
	// CServerSideClientBase::Connect( name='%s', userid=%d, fake=%d, connectiontypeflags=%d, chan->addr=%s )
	uint8 m_nConnectionTypeFlags;
	uint8 m_nAsyncDisconnectFlags; // check in Disconnect function, 1 add to queue, 2 disconnect now
	bool m_bMarkedToKick;
	SignonState_t m_nSignonState;
	bool m_bSplitScreenUser;
	bool m_bSplitAllowFastDisconnect;
	int m_nSplitScreenPlayerSlot;
	CServerSideClientBase* m_SplitScreenUsers[4];
	CServerSideClientBase* m_pAttachedTo;
	bool m_bSplitPlayerDisconnecting;
	int m_nDisconnectionTypeFlags;
	bool m_bFakePlayer;
	bool m_bInitialSpawnSent;	// init TRUE; controls whether initial spawn data was sent
	int m_nDisconnectSequence;	// set from server counter in Disconnect(), zeroed in Clear()

	CPlayerUserId m_UserID = -1;
	bool m_bReceivedPacket;	// true, if client received a packet after the last send packet
	CSteamID m_SteamID;
	CSteamID m_DisconnectedSteamID;
	CSteamID m_AuthTicketSteamID;
	CSteamID m_nFriendsID;
	ns_address m_nAddr;
	ns_address m_nAddr2;
	KeyValues* m_ConVars;
	CUtlDict<empty_t, int, k_eDictCompareTypeCaseInsensitiveFast> m_UnrecognizedConVarDict;

	bool m_bConVarsChanged;
	bool m_bConVarsInitialized;
	bool m_bIsHLTV;
	bool m_bClientInfoReceived;
	uint64 m_nPendingSpawnGroupMask; // bitmask of pending spawn group operations, cleared in Clear()
	uint32 m_nSendtableCRC;
	uint32 m_uChallengeNumber;
	int m_nSignonTick;
	int m_nDeltaTick;
	int m_nHltvReplayDeltaTick;		// init -1; delta tick used during HLTV replay mode
	int m_nStringTableAckTick;
	int m_nBaselineSequence;		// baseline sequence number for baseline ack processing
	CFrameSnapshot* m_pLastSnapshot;	// last send snapshot
	CUtlVector<void*> m_vecLoadedSpawnGroups;
	CMsgPlayerInfo m_playerInfo;
	CFrameSnapshot* m_pBaseline;
	int m_nBaselineUpdateTick;
	CBitVec<MAX_EDICTS>	m_BaselinesSent;
	int	m_nBaselineUsed;		// 0/1 toggling flag, singaling client what baseline to use
	int	m_nLoadingProgress;	// 0..100 progress, only valid during loading

	// This is used when we send out a nodelta packet to put the client in a state where we wait 
	// until we get an ack from them on this packet.
	// This is for 3 reasons:
	// 1. A client requesting a nodelta packet means they're screwed so no point in deluging them with data.
	//    Better to send the uncompressed data at a slow rate until we hear back from them (if at all).
	// 2. Since the nodelta packet deletes all client entities, we can't ever delta from a packet previous to it.
	// 3. It can eat up a lot of CPU on the server to keep building nodelta packets while waiting for
	//    a client to get back on its feet.
	int m_nForceWaitForTick = -1;

	CCircularBuffer m_TickHistory = {1024};	// stores recent tick data for delta computation and ack tracking
	bool m_bLowViolence = false;		// true if client is in low-violence mode (L4D server needs to know)
	bool m_bUsesSteamRelay = true;		// init TRUE; whether Steam Datagram Relay is used for this client
	bool m_bFullyAuthenticated = false;
	bool m_bUnk1 = false;

	// Time when we should send next world state update ( datagram )
	float m_fNextMessageTime = 0.0f;

	// Default time to wait for next message
	float m_fSnapshotInterval = 0.0f;

	float m_fAuthenticatedTime = -1.0f;
	float m_fUnkTime = 0.0f;

	// CSVCMsg_PacketEntities_t (CNetMessagePB<55,...>), sizeof 216 bytes
	CSVCMsg_PacketEntities_t m_PacketEntities;

	// CDelayedCallBase<CDelayedCallBase<CServerSideClientBase>>* pointers.
	CTSQueue<void*> m_DelayedCallQueue;

	// Queued message buffers
	CUtlVector<void*> m_PendingReliableMessages;
	CUtlVector<void*> m_PendingUnreliableMessages;

	// Command rate limiter
	int m_nMaxTicksWindow = 30;
	int m_nMaxCommandsPerWindow = 30;
	uint64 m_nMaxBytesPerWindow = 10240;
	int m_nLastResetTick = 0;
	int m_nCommandCount = 0;
	uint64 m_nByteCount = 0;

	CNetworkStatTrace m_Trace;

	// SV: Player %s kicked for too many failed console commands
	int m_spamCommandsCount = 0; // if the value is greater than 16, the player will be kicked with reason 39
	int m_nLastSpamCheckTick = 0;	// last tick at which the spam counter was evaluated/reset
	double m_lastExecutedCommand = 0.0; // if command executed more than once per second, ++m_spamCommandCount
	bool m_bPendingSignonStateMsg = false; // set when signon -> CONNECTED, consumed in ProcessSignonStateMsg
	CCommand* m_pCommand;
}; // sizeof 3024 (Windows), 3016 (Linux)

class CServerSideClient : public CServerSideClientBase
{
public:
	virtual ~CServerSideClient() = 0;

public:
	CPlayerBitVec m_VoiceStreams;
	CPlayerBitVec m_VoiceProximity;
	CCheckTransmitInfo m_PackInfo;
	CClientFrameManager m_FrameManager;
	void* m_pUnk3800;
	CClientFrame* m_pCurrentFrame;
	float m_flLastClientCommandQuotaStart = 0.0f;
	float m_flTimeClientBecameFullyConnected = -1.0f;
	bool m_bVoiceLoopback = false;
	bool m_bHltvReplayFromStash = false;
	bool m_bUnk10 = false;
	char _padEF3;
	int m_nHltvReplayDelay = 0;
	CHLTVServer* m_pHltvReplayServer;
	int m_nHltvReplayStopAt;
	int m_nHltvReplayStartAt;
	int m_nHltvReplaySlowdownBeginAt;
	int m_nHltvReplaySlowdownEndAt;
	float m_flHltvReplaySlowdownRate;
	int m_nHltvLastSendTick;
	double m_flHltvLastReplayRequestTime;
	CUtlVector<INetMessage*> m_HltvQueuedMessages;
	HltvReplayStats_t m_HltvReplayStats;
	void* m_pSendJob;
}; // sizeof 3968

class CHLTVClient : public CServerSideClientBase
{
public:
	virtual ~CHLTVClient() = 0;

public:
	CNetworkGameServerBase* m_pHLTV;
	CUtlString m_szPassword;
	CUtlString m_szChatGroup;
	double m_fLastSendTime = 0.0;
	double m_flLastChatTime = 0.0;
	int m_nUnknown1 = 0;
	int m_nSpawnGroupsSent = 0;
	int m_bNeedsFullFrame = 0;
	int m_nFullFrameTick = 0;
	int m_nLastSendTick = 0;
	bool m_bNoChat = false;
	bool m_bUnkBool = false;
	bool m_bUnkBool2 = false;
	void* m_pJob;
	CUtlVector<INetMessage*> m_QueuedMessages;
}; // sizeof 3120

#endif // SERVERSIDECLIENT_H
