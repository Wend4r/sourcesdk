//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ISERVER_H
#define ISERVER_H
#ifdef _WIN32
#pragma once
#endif

#include <edict.h>
#include <engine/IEngineService.h>
#include <engine/precache.h>
#include <framesnapshot.h>
#include <tier0/icvar.h>
#include <iloopmode.h>
#include <inetmsghandler.h>
#include <netadr.h>
#include <networkstringtabledefs.h>
#include <ns_address.h>
#include <playeruserid.h>
#include <qlimits.h>
#include <resourcefile/resourcetype.h>
#include <tier0/checksum_crc.h>
#include <steam/steamnetworkingtypes.h>
#include <networkbasetypes.pb.h>
#include <utlhash.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlstringtoken.h>
#include <tier1/utlhashmaplarge.h>
#include <ihltv.h>
#include <delayedcall.h>
#include <iclient.h>

enum server_state_t : int
{
	SS_Dead = 0,
	SS_WaitingForGameSessionManifest,
	SS_Loading,
	SS_Active,
	SS_Paused,
};

class IGameSpawnGroupMgr;
struct EventServerAdvanceTick_t;
struct EventServerPollNetworking_t;
struct EventServerProcessNetworking_t;
struct EventServerBeginSimulate_t;
struct EventServerEndSimulate_t;
struct EventServerPostSimulate_t;
struct SpawnGroupDesc_t;
class IPrerequisite;
class CServerChangelevelState;
class ISource2WorldSession;
class INetworkGameClient;
class KeyValues3;
class CPureServerWhitelist;
class CServerSideClientBase;
class CSVCMsg_ServerInfo_t;
class CSVCMsg_FlattenedSerializer_t;
class CCLCMsg_SplitPlayerConnect_t;
class C2S_CONNECT_Message;
class CNetworkStringTableContainer;
class CNetworkServerSpawnGroupCreatePrerequisites;
class CNetMessage;
class INetworkMessageInternal;
class CCompressedResourceManifest;
class IRecipientFilter;
class CBaselineEntityData;
class CNetworkServerSpawnGroup;
class CSteamID;
class IEntityReport;
class CServerSideClient;
class CHLTVServerAsync;
class CTvDirectDemoRecorder;
class CClientFrame;
class CJob;

typedef int ChallengeType_t;
typedef int PauseGroup_t;

using CUtlClientVector = CUtlVector< CServerSideClientBase *, CPlayerSlot >;

// Reservation bookkeeping that outlives a server instance: the game server points at a static
// copy, each SourceTV server owns one. The server keeps a pointer to every field.
struct ServerReservationState_t
{
	uint64 m_nReservationCookie;
	// -1 while clients are connected, the time the last one left otherwise
	float m_flTimeLastClientLeft;
	// Named by its place between the other two times: initialised to -1, nothing reads it in this build
	float m_flReservationSpareTime;
	float m_flReservationExpiryTime;
};

abstract_class INetworkGameServer 
{
public:
	virtual	void	Init( const GameSessionConfiguration_t &, const char * ) = 0;

	virtual void	SetGameSpawnGroupMgr( IGameSpawnGroupMgr * ) = 0;
	virtual void	SetGameSessionManifest( HGameResourceManifest ) = 0;

	virtual void	RegisterLoadingSpawnGroups( CUtlVector<unsigned int> & ) = 0;

	virtual void	Shutdown( void ) = 0;
	virtual void	AddRef( void ) = 0;
	virtual void	Release ( void ) = 0;

	virtual CGlobalVars *GetGlobals(void) = 0;

	virtual bool	IsActive( void ) const = 0;	
	virtual bool	IsPaused( void ) const = 0;	

	virtual void	SetServerTick( int tick ) = 0;
	// returns game world tick
	virtual int		GetServerTick( void ) const = 0;

	// returns current client limit
	virtual int		GetMaxClients( void ) const = 0;

	virtual void	ServerAdvanceTick( const EventServerAdvanceTick_t & ) = 0;
	virtual void	ServerPollNetworking( const EventServerPollNetworking_t & ) = 0;
	virtual void	ServerProcessNetworking( const EventServerProcessNetworking_t & ) = 0;

	virtual void	ServerBeginSimulate( const EventServerBeginSimulate_t & ) = 0;
	virtual void	ServerEndSimulate( const EventServerEndSimulate_t & ) = 0;
	virtual void	ServerPostSimulate( const EventServerPostSimulate_t & ) = 0;

	virtual SpawnGroupHandle_t LoadSpawnGroup( const SpawnGroupDesc_t & ) = 0;
	virtual void	AsyncUnloadSpawnGroup( unsigned int, /*ESpawnGroupUnloadOption*/ int ) = 0;
	virtual void	PrintSpawnGroupStatus( void ) const = 0;

	// returns the game time scale (multiplied in conjunction with host_timescale)
	virtual float	GetTimescale( void ) const = 0; 

	virtual bool	IsSaveRestoreAllowed( CUtlString *pReason ) const = 0;

	virtual void	SetMapName( const char *pszNewName ) = 0;
	// current map name (BSP)
	virtual const char *GetMapName( void ) const = 0;
	virtual const char *GetAddonName( void ) const = 0;

	virtual bool	IsBackgroundMap( void ) const = 0;

	// returns game world time (GetServerTick() * tick_interval)
	virtual float	GetTime( void ) const = 0;

	virtual bool	ActivateServer( void ) = 0;
	virtual bool	PrepareForAssetLoad( void ) = 0;

	virtual netadr_t GetServerNetworkAddress( void ) = 0;

	virtual SpawnGroupHandle_t FindSpawnGroupByName( const char *pszName ) = 0;
	virtual void	MakeSpawnGroupActive( SpawnGroupHandle_t ) = 0;
	virtual void	SynchronouslySpawnGroup( SpawnGroupHandle_t ) = 0;

	virtual void	SetServerState( server_state_t eNewState ) = 0;
	virtual server_state_t GetServerState( void ) = 0; // return m_State
	virtual bool	SpawnServer( const char * ) = 0;

	virtual int 	GetSpawnGroupLoadingStatus( SpawnGroupHandle_t ) = 0;
	virtual void	SetSpawnGroupDescription( SpawnGroupHandle_t, const char * ) = 0;

	virtual void*	StartChangeLevel( const char *pszMapName, const char *pszLandmark, const char *pszAddons ) = 0;
	virtual void	FinishChangeLevel( CServerChangelevelState * ) = 0;
	virtual bool	IsChangelevelPending( void ) const = 0;

	virtual void	GetAllLoadingSpawnGroups( CUtlVector< SpawnGroupHandle_t > &pOut ) = 0;

	virtual void	PreserveSteamID( void ) = 0;

	virtual CSVCMsg_GameSessionConfiguration &GetGameConfig() = 0;

	virtual void	ReserveServerForQueuedGame( const char *pszReason ) = 0;

	virtual bool	IsReserved() = 0; // return m_nReservationCookie != 0
	virtual bool	IsMultiplayer() = 0; // return m_bIsMultiplayer
	virtual bool	IsPlayingSoloAgainstBots() = 0;

	virtual void	BroadcastPrintf( const char *pszFmt, ... ) FMTFUNCTION( 2, 3 ) = 0;

	virtual void	SetClientConnect( CPlayerSlot slot, bool b = true ) = 0;
	virtual SignonState_t	GetClientSignonState( CPlayerSlot slot ) = 0;

	virtual void	NotifySceneViewDebugOverlays( void *pSceneView, bool bUnk ) = 0;

	virtual void	BroadcastMessage( INetworkMessageInternal *pSerializer, const CNetMessage *pNetMessage, const IRecipientFilter *pFilter ) = 0;
	virtual bool	IsRecordingDemo() = 0;

	virtual uint8	GetClientConnectionType( CPlayerSlot slot ) = 0;

	virtual bool	HasReplayDirector() = 0;
	virtual float	GetAverageFrameTime() = 0;

	virtual void	PrepareSendClientUpdatesMainThread() = 0;
	virtual void 	PrepareSendClientUpdatesAsync() = 0;

	virtual CSteamID	GetGameServerSteamID() = 0;

	virtual bool BroadcastVoiceData( int nEntityIndex, void *pVoiceData, uint64 xuidFrom ) = 0;
};

class CNetworkGameServerBase : public INetworkGameServer, protected IConnectionlessPacketHandler, protected IConVarListener
{
public:
	virtual ~CNetworkGameServerBase() = 0;

	virtual void	SetMaxClients( int nMaxClients ) = 0;
	virtual CPlayerSlot	CreateClient( CPlayerSlot nSlot, CSteamID nSteamID, const char *pszName) = 0;

public: // IConnectionlessPacketHandler
	virtual bool	ProcessConnectionlessPacket( netpacket_t *packet ) = 0; // process a connectionless packet

public: // IConVarListener
	virtual void	OnConVarCreated( ConVarRefAbstract *pNewCvar ) = 0;
	virtual void	OnConCommandCreated( ConCommand *pNewCommand ) = 0;

public:
	virtual CPlayerUserId GetPlayerUserId( CPlayerSlot slot ) = 0;
	virtual const char *GetPlayerNetworkIDString( CPlayerSlot slot ) = 0;
	
	// Returns udp port of this server instance
	virtual uint16 GetUDPPort() = 0;
	// Returns hostname of this server instance
	virtual const char *GetName() = 0;

	// AMNOTE: arg names are speculative and might be incorrect!
	// Sums up across all the connected players.
	virtual void	GetNetStats( float &inflow, float &outflow ) = 0;

	virtual void	FillKV3ServerInfo( KeyValues3 *out ) = 0;

	virtual bool	IsHLTV() = 0;
	
	virtual bool	IsPausable( PauseGroup_t ) = 0;

	// Returns sv_password cvar value, if it's set to "none" nullptr would be returned!
	virtual const char *GetPassword() = 0;

	virtual void	RemoveClientFromGame(CServerSideClientBase *, /*ENetworkDisconnectionReason*/ int ) = 0;

	virtual void	FillServerInfo( CSVCMsg_ServerInfo_t *pServerInfo ) = 0;
	virtual void	UserInfoChanged( CPlayerSlot slot ) = 0;

	// 2nd arg is unused.
	virtual void	GetClassBaseline( void *, ServerClass *pClass, intp *pOut ) = 0;

	virtual void	StartHLTVMaster() = 0;

	virtual CServerSideClientBase *ConnectClient( const char *pszName, ns_address *pAddr, HSteamNetConnection hPeer, const C2S_CONNECT_Message &msg, 
	                                              const char *pszDecryptedPassword, const byte *pAuthTicket, int nAuthTicketLength, bool bIsLowViolence ) = 0;
	virtual CServerSideClientBase *CreateNewClient( CPlayerSlot slot ) = 0;
	
	virtual bool	FinishCertificateCheck( const ns_address *pAddr, int socket, byte ) = 0;

	virtual ChallengeType_t	GetChallengeType( const ns_address &addr ) = 0;
	virtual bool	CheckPassword( const ns_address &addr, const char *password ) = 0;

	virtual void	CalculateCPUUsage() = 0;

	const CUtlClientVector &GetClients() const { return m_Clients; }
	CServerSideClientBase *GetClientBySlot_unsafe( CPlayerSlot slot ) const { return GetClients()[slot]; }
	CServerSideClientBase *GetClientBySlot( CPlayerSlot slot ) const
	{
		if ( !slot.IsValid() )
		{
			return nullptr;
		}

		const auto &vecClients = GetClients();

		if ( !( 0 <= slot && slot < vecClients.Count() ) )
		{
			return nullptr;
		}

		return vecClients[slot];
	}

public:
	// The same resource group bookkeeping as the client's
	typedef ::ResourceGroupRef_t ResourceGroupRef_t;
	typedef ::ResourceGroupVector_t ResourceGroupVector_t;

	ResourceGroupVector_t m_ResourceGroups;

	server_state_t m_State;
	int m_nRefCount;
	int m_Socket;
	int m_nTickCount;
	// Clamped from the game session configuration by SetMaxClients
	int m_nMinClientsLimit;
	int m_nMaxClientsLimit;
	// One bit per PauseGroup_t
	int m_nPauseFlags;

	CGlobalVars m_globals;
	GameSessionConfiguration_t m_GameConfig;

	CUtlString m_szMapname;
	CUtlString m_szAddons;
	// Copied from sv_skyname by ActivateServer
	CUtlString m_szSkyname;
	// Named by what is known of it: constructed and freed, never set in this build
	CUtlString m_szUnreferencedString;

	KeyValues *m_pLaunchOptions;

	CNetworkStringTableContainer *m_pStringTables;
	// The second container PrepareForAssetLoad sets up next to m_pStringTables
	CNetworkStringTableContainer *m_pStringTablesSecondary;
	INetworkStringTable *m_pInstanceBaselineTable;
	INetworkStringTable *m_pUserInfoTable;
	INetworkStringTable *m_pServerQueryInfoTable;

	CFrameSnapshotManager *m_pFrameSnapshotManager;

	// Guards m_InstanceBaselines, GetClassBaseline looks baselines up by ServerClass instance baseline index
	CThreadFastMutex m_InstanceBaselinesMutex;
	CUtlHashMap< uint32, CBaselineEntityData * > m_InstanceBaselines;

	bf_write m_Signon;
	CUtlLeanVector< byte > m_SignonBuffer;

	bool m_bIsBackgroundMap;
	CUtlClientVector m_Clients;
	CCompressedResourceManifest *m_pResourceManifest;

	CThreadFastMutex m_BaselineEntitiesMutex;
	CUtlHashtable< uint32, CBaselineEntityData * > m_BaselineEntitiesMap;

	SpawnGroupHandle_t m_hActiveSpawnGroup;
	int m_nMaxClients;
	int m_nSpawnCount;
	int m_nServerClasses;
	int m_nServerClassBits;
	uint64 m_nCreateTime;

	bool m_bPendingChangeLevel;
	// Set by FinishChangeLevel, ActivateServer keeps the Steam ID only after a finished changelevel
	bool m_bFinishedChangeLevel;
	bool m_bPreserveSteamID;
	bool m_bHibernating;

	// Calls queued for the main thread, such as starting a SourceTV recording
	CUtlVector< CDelayedCallBase< CNetworkGameServerBase > * > m_DelayedCalls;

	CUtlLinkedList< CNetworkServerSpawnGroupCreatePrerequisites * > m_SpawnGroupPrerequisites;
	CUtlMap< SpawnGroupHandle_t, CNetworkServerSpawnGroup * > m_SpawnGroups;

	CUtlVector< uint64 > m_QueuedSpawnGroupUnloads; // SpawnGroupUnloadEntry_t (handle + flags)
	// AsyncUnloadSpawnGroup queues the unload while this is non-zero
	int m_nQueueSpawnGroupUnloads;
	SpawnGroupHandle_t m_hJustInTimeResourceSpawnGroup;

	// The Steam game data string (sv_search_key, sv_steamgroup, ...)
	CUtlVector< char > m_GameData;
	CUtlString m_szTags;
	// Named by what it does: incremented on every client disconnect, the client stores the new value
	int m_nDisconnectSequence;
	// Named by what is known of it: constructed and freed, never filled in this build
	CUtlVector< byte > m_UnreferencedBytes;

	// The client whose command runs right now, nullptr for the server console
	CServerSideClientBase *m_pCommandClient;
	IGameSpawnGroupMgr *m_pSpawnGroupMgr;
	ISource2WorldSession *m_pWorldSession;
	HGameResourceManifest m_hGameSessionManifest;
	// Named by what it does: set to 1 by SetGameSpawnGroupMgr
	int m_nSpawnGroupMgrAssigned;

	// Point into a ServerReservationState_t that outlives the server
	uint64 *m_pnReservationCookie;
	float *m_pflTimeLastClientLeft;
	float *m_pflReservationSpareTime;
	float *m_pflReservationExpiryTime;

	// Counts OnSpawnGroupActivate calls, reset by SetGameSpawnGroupMgr
	int m_nSpawnGroupCreationSequence;
	// The resource system's count of just-in-time resources when the spawn group was last rebuilt
	int m_nJustInTimeResourceCount;

	// handle = slot | (serial << 8). Init all 0xFFFF.
	uint16 m_nClientSlotSerial[ ABSOLUTE_PLAYER_LIMIT ];

	// Incremented whenever m_GameData changes
	int m_nGameDataVersion;
	int m_numGameSlots;
	float m_fLastCPUCheckTime;
	float m_fCPUPercent;
	float m_flTimescale;
	// Named by what it does: set when a client's peer list changes, the next tick sends CSVCMsg_PeerList
	bool m_bPeerListDirty;
	bool m_bIsMultiplayer;
	// Named by what is known of it: initialised to false, nothing reads it in this build
	bool m_bUnreferencedFlag;

	int m_nFrameTimeBucketCount;
	uint64 m_FrameTimeBuckets[ 11 ];
	int64 m_nFrameTimeSum;
	int64 m_nFrameTimeCount;

	int m_nComputeTimeBucketCount;
	uint64 m_ComputeTimeBuckets[ 61 ];
	double m_flComputeTimeSum;
	uint64 m_nComputeTimeCount;

	uint64 m_nReservationCookie;
	// "Immediate demo recorder", started without SourceTV
	CTvDirectDemoRecorder *m_pImmediateDemoRecorder;
	// The size of the user info convar data last given to the clients, OnConVarCreated grows their buffers
	uint64 m_nUserInfoConVarDataSize;
	// Named by what it does: counts active clients whose connection type has bit 0 set
	int m_nFlaggedConnectionTypeClients;
	void *m_pReplayDirector;
};

class CNetworkGameServer : public CNetworkGameServerBase {
public:
	CUtlString m_szStartspot;
	bf_write m_FullSendTables;
	CUtlLeanVector<byte> m_FullSendTablesBuffer; // buffer for m_FullSendTables
	int m_nMaxPrecacheEntries;
	CPrecacheItem m_Precache[MAX_GENERIC];
	INetworkStringTable* m_pGenericPrecacheTable;
	bool m_bAllowSignonWrites;
};


//-----------------------------------------------------------------------------
// Purpose: Frames of a SourceTV server waiting to be broadcast, newest tick first.
//-----------------------------------------------------------------------------
struct HLTVFrameCache_t
{
	// The tick the cache was last advanced to
	int m_nTick;
	int m_nCount;
	int m_nNext;
	CClientFrame *m_pFrames[ 16384 ];
};

//-----------------------------------------------------------------------------
// Purpose: SourceTV server. Names from the CS:GO CHLTVServer where the field does the same.
//-----------------------------------------------------------------------------
class CHLTVServer : public CNetworkGameServerBase, public IHLTVServer, public ITvStreamSink
{
public: // IHLTVServer, in the order the class declares them
	virtual CNetworkGameServerBase *GetBaseServer( void ) = 0;
	virtual IHLTVDirector *GetDirector( void ) = 0;
	virtual CPlayerSlot GetHLTVSlot( void ) = 0;
	virtual float GetOnlineTime( void ) = 0;
	virtual void GetLocalStats( int &proxies, int &slots, int &specs ) = 0;
	virtual void GetGlobalStats( int &proxies, int &slots, int &specs ) = 0;
	virtual bool IsMasterProxy( void ) = 0;
	virtual bool IsTVRelay( void ) = 0;
	virtual bool IsDemoPlayback( void ) = 0;
	virtual bool IsActive( void ) = 0;
	virtual void BroadcastEvent( INetworkMessageInternal *pEvent, const CNetMessage *pData ) = 0;
	virtual bool IsRecording( void ) = 0;
	virtual int StartAutoRecording( void ) = 0;
	virtual const char *GetRecordingDemoFilename( void ) = 0;
	virtual int GetBroadcastStartTick( void ) = 0;
	virtual void StopRecording( const CGameInfo *pInfo ) = 0;
	virtual void AddSaveGame( CSteamID steamID, uint64, int, const CUtlBuffer &buf ) = 0;
	virtual const netadr_t *GetPublicAddress( void ) = 0;
	virtual const netadr_t *GetRelayAddress( void ) = 0;
	virtual void GetRelayStats( int &proxies, int &slots, int &specs ) = 0;

public: // ITvStreamSink, in the order the class declares them
	virtual bool SendNetMessage( const CNetMessage *pData, bool bReliable ) = 0;
	virtual bool BeginFrame( const CClientFrame *pFrame ) = 0;
	virtual void AddFrame( const CClientFrame *pFrame ) = 0;
	virtual CJob *GetSendJob( void ) = 0;
	virtual int GetLastFrameTick( void ) = 0;

public:
	// Named by what is known of it: constructed and freed, never set in this build
	CUtlString m_szUnreferencedTVString;
	ServerReservationState_t m_ReservationState;
	CHLTVServerAsync *m_pAsync;
	// The SourceTV client on the game server, set on the master proxy only
	CServerSideClient *m_MasterClient;
	// The game server m_MasterClient is connected to
	CNetworkGameServerBase *m_Server;
	CHLTVClientState m_ClientState;
	IHLTVDirector *m_Director;
	// Slot of the SourceTV client on the game server
	CPlayerSlot m_nPlayerSlot;
	HLTVFrameCache_t m_FrameCache;
	// Set on every SourceTV server by svc_PacketReliable
	bool m_bPacketReliableReceived;
	// Cleared when the master proxy starts
	bool m_bSignonState;
	// Clamps the client limit to 1 when the master proxy starts
	bool m_bMasterOnlyMode;
	// Named by what is known: zeroed by the constructor, nothing reads it in this build
	uint16 m_nUnreferencedFlags;
	float m_flStartTime;
	float m_flFPS;
	int m_nGameServerMaxClients;
	// Named by what is known: zeroed by the constructor, nothing reads it in this build
	int m_nUnreferencedInt;
	netadr_t m_RootServer;
	int m_nGlobalSlots;
	int m_nGlobalClients;
	int m_nGlobalProxies;
	int m_nExternalTotalViewers;
	int m_nExternalLinkedViewers;
	// Named by what is known: zeroed with the viewer counts, nothing reads it in this build
	int m_nExternalReserved;
	bf_write m_DemoEventWriteBuffer;
	CUtlLeanVector< byte > m_DemoEventBufferMemory;
	float m_fNextSendUpdateTime;
	byte m_SignonBufferMemory[ 98304 ];
	// Swapped with the asynchronous half's buffer when a frame begins
	bf_write m_SignonBuffer;
	// Constructor argument; relays honour tv_relay_hard_shutdown with it
	bool m_bRelayHardShutdown;
	int m_nInstanceIndex;
};

abstract_class INetworkServerService : public IEngineService
{
public:
	virtual CNetworkGameServer	*GetNetworkServer( void ) = 0;
	CNetworkGameServerBase *GetIGameServer( void ) { return static_cast<CNetworkGameServerBase *>( GetNetworkServer() ); }
	// Server state is at least SS_Active
	virtual bool	IsActiveInGame( void ) const = 0;
	// More than one client slot
	virtual bool	IsMultiplayer( void ) const = 0;
	virtual void	StartupServer( const GameSessionConfiguration_t &config, ISource2WorldSession *pWorldSession, const char *pszLevelName ) = 0;
	virtual void	SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;
	// pszResourceGroupName is loaded as the "Server GameSessionManifest" resource manifest group
	virtual void	AddServerPrerequisites( const GameSessionConfiguration_t &config, const char *pszResourceGroupName, ILoopModePrerequisiteRegistry *pRegistry, bool bChangelevel ) = 0;
	virtual bool	IsServerRunning( void ) const = 0;
	// Also shuts down the SourceTV servers
	virtual void	DisconnectGameNow( ENetworkDisconnectionReason ) = 0;
	// Logs "No server loaded" without a server
	virtual void	PrintSpawnGroupStatus( void ) const = 0;
	virtual netadr_t	GetServerNetworkAddress( void ) = 0;
	virtual bool	GameLoadFailed( void ) const = 0;
	virtual void	SetGameLoadFailed( bool bFailed ) = 0;
	virtual void	SetGameLoadStarted( void ) = 0;
	virtual void	StartChangeLevel( const char *pszMapName, const char *pszLandmark, const char *pszAddons ) = 0;
	virtual bool	FinishChangeLevel( void ) = 0;
	virtual bool	IsChangeLevelPending( void ) const = 0;
	virtual void	PreserveSteamID( void ) = 0;
	virtual CRC32_t	GetServerSerializersCRC( void ) = 0;
	// Built on first use from the server's flattened serializers
	virtual CSVCMsg_FlattenedSerializer_t *GetServerSerializersMsg( void ) = 0;
	virtual IGameSpawnGroupMgr *GetGameSpawnGroupMgr( void ) = 0;
	virtual bool	IsSaveRestoreAllowed( CUtlString *pError ) = 0;
	virtual bool	IsEntityReportActive( void ) = 0;
	// Returns nullptr unless the entity report is active and targets this slot or every slot
	virtual IEntityReport *GetEntityReport( int nSlot ) = 0;
	// Empty in this build, named by where they are called: a begin/end pair around
	// ISource2Server::GameFrame on the secondary main thread (outside EnterSyncInterval/ExitSyncInterval)
	// and around closing a demo file.
	virtual void	BeginSecondaryThreadWork( void ) = 0;
	virtual void	EndSecondaryThreadWork( void ) = 0;
	virtual bool	ThreadInPrimaryOrSecondaryMainThread( void ) = 0;
	virtual void	EnterSyncInterval( void ) = 0;
	// AMNOTE: The bool picks which of two parallel work modes clients enter
	virtual void	ExitSyncInterval( bool ) = 0;
};

typedef CNetworkGameServerBase IServer;


#endif // ISERVER_H
