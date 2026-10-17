#ifndef SERVERSIDECLIENTBASE_H
#define SERVERSIDECLIENTBASE_H

#if COMPILER_MSVC
#pragma once
#endif

#include <networkgameserverspawngroupautolock.h>
#include <networksystem/inetchannel.h>
#include <playerslot.h>
#include <playeruserid.h>
#include <delayedcall.h>
#include <threadedjob.h>

#include <entity2/entityidentity.h>
#include <steam/steamclientpublic.h>
#include <engine/clientframe.h>
#include <tier0/annotations.h>
#include <tier0/bufferstring.h>
#include <tier0/circularbuffer.h>
#include <tier0/utlstring.h>
#include <tier0/utlsignalslot.h>
#include <tier0/bitbuf.h>
#include <tier1/ns_address.h>
#include <networksystem/inetworksystem.h>

#include <netmessages.h>
#include <utldict.h>

class INetMessage;
class CNetworkGameServerBase;
class CNetworkGameServer;
class CNetworkServerSpawnGroup;

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
class CCLCMsg_Diagnostic_t;
class CCLCMsg_Move_t;
class CCLCMsg_VoiceData_t;
class CCLCMsg_FileCRCCheck_t;
class CCLCMsg_RespondCvarValue_t;
class NetMessagePacketStart_t;
class NetMessagePacketEnd_t;
class NetMessageConnectionClosed_t;
class NetMessageConnectionCrashed_t;
class NetMessageSplitscreenUserChanged_t;
class ITvStreamSink;

struct Spike_t
{
public:
	CUtlString m_szDesc;
	int m_nBits;
};
COMPILE_TIME_ASSERT( sizeof( Spike_t ) == 16 );

class CNetworkStatTrace
{
public:
	CUtlVector<Spike_t> m_Records;
	int m_nMinWarningBytes;
	int m_nStartBit;
	int m_nCurBit;
};
COMPILE_TIME_ASSERT( sizeof( CNetworkStatTrace ) == 40 );

// AMNOTE: While the client's send job runs, the deferred methods (Disconnect, SendNetMessage, SendInitialSpawnGroups,
// StopHltvReplay, ...) are queued in m_DelayedCallQueue
enum ClientSendJobState_t : uint8
{
	CLIENT_SEND_JOB_IDLE = 0,
	CLIENT_SEND_JOB_RUNNING,
	// AMNOTE: The job is done, but its queued calls aren't flushed yet
	CLIENT_SEND_JOB_FINISHED,
};

// AMNOTE: While it's not CLIENT_SLOT_RECYCLABLE, the slot shows as reserved and keeps its address on disconnect
enum ClientSlotReservation_t : int
{
	CLIENT_SLOT_RECYCLABLE = 0,
	// AMNOTE: INetworkGameServer::SetRecyclePlayerSlot switches between this and CLIENT_SLOT_RECYCLABLE,
	// but leaves CLIENT_SLOT_CREATED as is
	CLIENT_SLOT_NOT_RECYCLABLE,
	// AMNOTE: Set by CServerSideClientBase::Create
	CLIENT_SLOT_CREATED,
};

class CServerSideClientBase : public CUtlSlot, public INetworkChannelNotify, public INetworkMessageProcessingPreFilter
{
public:
	// Async payload for CNETMsg_SpawnGroup_SetCreationTick_t
	struct OnSpawnGroupActivateCall_t
	{
		int m_nCreationTick; // msg.tick
		SpawnGroupHandle_t m_hSpawnGroup; // msg.spawngrouphandle
		int m_nCreationSequence; // msg.creationsequence
		bool m_b; // producer always sets true
	};

	// Async payload for CNETMsg_SpawnGroup_Unload_t
	struct OnSpawnGroupDeactivateCall_t
	{
		int m_nCreationTick; // current server tick
		SpawnGroupHandle_t m_hSpawnGroup; // msg.spawngrouphandle
		uint32 m_nUnloadFlags; // producer always sets 0 (Unload flags/tickcount)
		bool m_b; // producer always sets true
	};

	struct BuildServerInfoMessageAsync_t
	{
		bool m_bAsync; // Built by the delayed call, otherwise synchronously
		int m_nTick;
		int m_nPlayers;
		int m_nHumans;
		int m_nBots;
		int m_nSpawnCount;
		CUtlVector< CNetworkServerSpawnGroup * > m_vecInitialSpawnGroups;
		CNETMsg_SetConVar_t m_SetConVar;
	};

	struct SendNetMessageCall_t
	{
		CNetMessage *m_pMessage;
		NetChannelBufType_t m_BufferType;
	};

	struct OnSpawnGroupLoadCall_t
	{
		void *m_pUnk0000;
		bool m_bAsync; // logged as "Sync"/"Async"
	};

	// Async payload for CNETMsg_SpawnGroup_ManifestUpdate_t
	struct SendSpawnGroupManifestUpdateCall_t
	{
		CBufferString m_sManifest;
		SpawnGroupHandle_t m_hSpawnGroup;	// msg.spawngrouphandle
		bool m_bUnk0014;
	};

	// Copied out of CCLCMsg_BaselineAck at enqueue time (not a message pointer)
	struct ProcessBaselineAckCall_t
	{
		int m_nBaselineTick; // msg.baseline_tick
		int m_nBaselineNr; // msg.baseline_nr (index, 0/1)
	};

	struct UpdateAcknowledgedFramecountCall_t
	{
		int m_nTick;
	};

	struct SignonStateNewCall_t
	{
		int m_nMaxClients;
	};

	struct SignonStateNewFailedCall_t
	{
	};

	struct DisconnectCall_t
	{
		ENetworkDisconnectionReason m_Reason;
		CUtlString m_sInternalReason;
	};

	using ThisClass = CServerSideClientBase;
	using BaseClass = CServerSideClientBase;

	DECLARE_DELAYED_CALL( OnSpawnGroupActivateDelayedCall_t, OnSpawnGroupActivateCall_t, CNetworkGameServerSpawnGroupAutoLock );
	DECLARE_DELAYED_CALL( OnSpawnGroupDeactivateDelayedCall_t, OnSpawnGroupDeactivateCall_t, CNetworkGameServerSpawnGroupAutoLock );
	DECLARE_DELAYED_CALL( BuildServerInfoMessageDelayedCall_t, BuildServerInfoMessageAsync_t, CNetworkGameServerSpawnGroupAutoLock );
	DECLARE_DELAYED_CALL( SendNetMessageDelayedCall_t, SendNetMessageCall_t, empty_t );
	DECLARE_DELAYED_CALL( OnSpawnGroupLoadDelayedCall_t, OnSpawnGroupLoadCall_t, CNetworkGameServerSpawnGroupAutoLock );
	DECLARE_DELAYED_CALL( SendSpawnGroupManifestUpdateDelayedCall_t, SendSpawnGroupManifestUpdateCall_t, CNetworkGameServerSpawnGroupAutoLock );
	DECLARE_DELAYED_CALL( ProcessBaselineAckDelayedCall_t, ProcessBaselineAckCall_t, empty_t );
	DECLARE_DELAYED_CALL( UpdateAcknowledgedFramecountDelayedCall_t, UpdateAcknowledgedFramecountCall_t, empty_t );
	DECLARE_DELAYED_CALL( SignonStateNewDelayedCall_t, SignonStateNewCall_t, empty_t );
	DECLARE_DELAYED_CALL( SignonStateNewFailedDelayedCall_t, SignonStateNewFailedCall_t, empty_t );
	DECLARE_DELAYED_CALL( DisconnectDelayedCall_t, DisconnectCall_t, empty_t );

	virtual ~CServerSideClientBase() = 0;

public:
	CPlayerSlot              GetPlayerSlot() const { return m_nClientSlot; }
	CPlayerUserId            GetUserID() const { return m_UserID; }
	CEntityIndex             GetEntityIndex() const { return m_nEntityIndex; }
	CSteamID                 GetClientSteamID() const { return m_SteamID; }
	const char              *GetClientName() const { return m_Name; }
	INetChannel             *GetNetChannel() const { return m_pNetChannel; }
	ClientSlotReservation_t  GetSlotReservation() const { return m_nSlotReservation; }
	const netadr_t          *GetRemoteAddress() const { return m_Addr.Get<netadr_t>(); }
	CNetworkGameServerBase  *GetServer() const { return m_pGameServer; }

	virtual void             Connect( int socket, const char* pszName, int nUserID, INetChannel* pNetChannel, uint8 nConnectionTypeFlags, uint32 uChallengeNumber ) = 0; // bool bFakePlayer = !nConnectionTypeFlags || (nConnectionTypeFlags & 8) != 0;
	virtual void             Inactivate( const char *pszAddons ) = 0;
	virtual void             Reactivate( CPlayerSlot nSlot ) = 0;
	virtual void             SetServer( CNetworkGameServer *pNetServer ) = 0;
	virtual void             Reconnect() = 0;
	virtual void             Disconnect( ENetworkDisconnectionReason reason, const char *pszInternalReason ) = 0;
	virtual bool             CheckConnect() = 0;
	virtual CPlayerSlot      Create( CSteamID nSteamID, const char *pszName ) = 0; // Fake clients: sets the steamid and name (SetName), returns the slot
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
	virtual bool             ProcessDiagnostic( const CCLCMsg_Diagnostic_t &msg ) = 0; // Forwards to ISource2GameClients::ClientDiagnostic

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
	void                     ExecuteDelayedCall( empty_t & ) {}
	virtual void             ExecuteDelayedCall( BuildServerInfoMessageAsync_t &call ) = 0;
	void                     ExecuteDelayedCall( OnSpawnGroupActivateCall_t &call );
	void                     ExecuteDelayedCall( OnSpawnGroupDeactivateCall_t &call );
	void                     ExecuteDelayedCall( SendNetMessageCall_t &call );
	void                     ExecuteDelayedCall( OnSpawnGroupLoadCall_t &call );
	void                     ExecuteDelayedCall( SendSpawnGroupManifestUpdateCall_t &call );
	void                     ExecuteDelayedCall( ProcessBaselineAckCall_t &call );
	void                     ExecuteDelayedCall( UpdateAcknowledgedFramecountCall_t &call );
	void                     ExecuteDelayedCall( SignonStateNewCall_t &call );
	void                     ExecuteDelayedCall( SignonStateNewFailedCall_t &call );
	void                     ExecuteDelayedCall( DisconnectCall_t &call );

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

	// AMNOTE: Counts the players sharing this client's connection, itself included (non-null m_SplitScreenUsers).
	// Split screen users forward to the client they're attached to
	virtual int              GetSplitScreenPlayerCount() = 0;

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

	// AMNOTE: Called once the delayed calls queued during the send job are flushed, and when the send job starts
	virtual void             OnSendJobFinished() = 0;
	virtual void             OnSendJobStarted() = 0;

	virtual bool             ProcessSignonStateMsg( SignonState_t state, int nSpawnCount ) = 0; // CNETMsg_SignonState::signon_state, ::spawn_count
	virtual void             PerformDisconnection( ENetworkDisconnectionReason reason ) = 0;

public:
	// AMNOTE: "BOT", "STEAM_ID_PENDING", "STEAM_ID_LAN" or the SteamID as text
	CUtlString m_NetworkIDString;
	CUtlString m_Name;
	CPlayerSlot m_nClientSlot;
	CEntityIndex m_nEntityIndex;
	CNetworkGameServerBase *m_pGameServer;
	INetChannel *m_pNetChannel;
	// CServerSideClientBase::Connect( name='%s', userid=%d, fake=%d, connectiontypeflags=%d, chan->addr=%s )
	uint8 m_nConnectionTypeFlags;
	ClientSendJobState_t m_nSendJobState;

	// AMNOTE: Set on construction, cleared by OnSendJobStarted and set again by OnSendJobFinished,
	// so it's false while the client's send job runs
	bool m_bSendJobIdle;
	SignonState_t m_nSignonState;
	bool m_bSplitScreenUser;
	// AMNOTE: A split screen user goes through the full disconnection only when it's set, otherwise the server sends "ss_remove"
	bool m_bSplitAllowFastDisconnect;
	int m_nSplitScreenPlayerSlot;
	CServerSideClientBase *m_SplitScreenUsers[4];
	CServerSideClientBase *m_pAttachedTo;
	bool m_bSplitPlayerDisconnecting;

	ClientSlotReservation_t m_nSlotReservation;
	bool m_bFakePlayer;
	// AMNOTE: Full updates are logged with their reason while it's set
	bool m_bLogFullUpdate;
	// AMNOTE: Snapshots sent without a delta frame
	int m_nFullUpdateCount;

	CPlayerUserId m_UserID = -1;
	bool m_bReceivedPacket;	// true, if client received a packet after the last send packet
	CSteamID m_SteamID;
	// AMNOTE: Kept when the client is cleared, a reconnecting player gets the slot that has their SteamID back
	CSteamID m_DisconnectedSteamID;
	// AMNOTE: The SteamID of the auth session, ended on disconnect
	CSteamID m_AuthTicketSteamID;
	CSteamID m_nFriendsID;
	ns_address m_Addr;
	// AMNOTE: Kept on disconnect, a reconnecting player gets the slot that has their address back
	ns_address m_DisconnectedAddr;

	// AMNOTE: Set from a server-wide counter on disconnect, the disconnected slot with the lowest one is reused first
	int m_nDisconnectSequence;

	KeyValues *m_pConVars;
	CUtlDict< empty_t, int, k_eDictCompareTypeCaseInsensitiveFast > m_UnrecognizedConVarDict;

	bool m_bConVarsChanged;
	bool m_bConVarsInitialized;
	bool m_bIsHLTV;

	// AMNOTE: Set when the server removes the TV sink
	bool m_bTvStreamSinkRemoved;

	// AMNOTE: The HLTV client's downstream TV sink, SendNetMessage forwards to it
	ITvStreamSink *m_pTvStreamSink;
	uint32 m_nSendtableCRC;
	uint32 m_uChallengeNumber;
	int m_nSignonTick;
	int m_nDeltaTick;

	// AMNOTE: Tick of the last delta snapshot sent reliably while sv_disable_reliable_delta_retransmit is set,
	// snapshots delta from it as if the client acknowledged it
	int m_nLastReliableDeltaTick;
	int m_nStringTableAckTick;
	int m_nBaselineSequence;		// baseline sequence number for baseline ack processing
	CFrameSnapshot *m_pLastSnapshot;	// last send snapshot
	CUtlVector< SpawnGroupHandle_t > m_vecLoadedSpawnGroups;
	CMsgPlayerInfo m_playerInfo;
	CFrameSnapshot *m_pBaseline;
	int m_nBaselineUpdateTick;
	CBitVec< MAX_EDICTS > m_BaselinesSent;
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

	// AMNOTE: The client connected through a loopback address
	bool m_bLoopback = false;
	bool m_bFullyAuthenticated = false;
	bool m_bClanOfficer = false;
	bool m_bClanMember = false;
	bool m_bInitialSpawnGroupsSent = false;

	// Time when we should send next world state update ( datagram )
	float m_fNextMessageTime = 0.0f;

	// Default time to wait for next message
	float m_fSnapshotInterval = 0.0f;

	// AMNOTE: Plat_FloatTime when Steam canceled the auth ticket, -1 when there's none pending
	float m_flAuthTicketCanceledTime = -1.0f;

	// AMNOTE: Plat_FloatTime of the last "userinfo ignored" warning, which is printed at most every 10 seconds
	float m_flLastUserInfoIgnoredTime = 0.0f;

	// CSVCMsg_PacketEntities_t (CNetMessagePB<55,...>), sizeof 216 bytes
	CSVCMsg_PacketEntities_t m_PacketEntities;

	// CDelayedCallBase<CDelayedCallBase<CServerSideClientBase>>* pointers.
	CTSQueue< void * > m_DelayedCallQueue;

	// AMNOTE: The send job runs the delayed calls of the first, which it swaps in from the second, where they're queued meanwhile
	CUtlVector< CDelayedCallBase< CServerSideClientBase > * > m_SendJobDelayedCalls;
	CUtlVector< CDelayedCallBase< CServerSideClientBase > * > m_PendingSendJobDelayedCalls;

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
	double m_lastExecutedCommand = 0.0; // if command executed more than once per second, ++m_spamCommandCount
	bool m_bPendingSignonStateMsg = false; // set when signon -> CONNECTED, consumed in ProcessSignonStateMsg
	CCommand *m_pLastCommand;
};

#endif // SERVERSIDECLIENTBASE_H
