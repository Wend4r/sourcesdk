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
#include <utlhashmaplarge.h>
#include <ihltv.h>

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
class ISceneViewDebugOverlays;
class IHLTVDirector;
class CHLTVServerAsync;
class CTvDirectDemoRecorder;

typedef int ChallengeType_t;
typedef int PauseGroup_t;

using CUtlClientVector = CUtlVector< CServerSideClientBase *, CPlayerSlot >;

struct SplitDisconnect_t {
	CServerSideClientBase* m_pUser;
	CServerSideClientBase* m_pSplit;
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

	virtual CServerChangelevelState *StartChangeLevel( const char *pszMapName, const char *pszLandmark, const char *pszAddons ) = 0;
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

	// Adds (or removes) the engine's scene view debug overlays listener to the given overlays
	virtual void	NotifySceneViewDebugOverlays( ISceneViewDebugOverlays *pDebugOverlays, bool bAddListener ) = 0;

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

	// 2nd arg is unused. Returns false when the class has no instance baseline yet
	virtual bool	GetClassBaseline( void *, ServerClass *pClass, intp *pOut ) = 0;

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
	struct ResourceGroupVector_t
	{
		CUtlVector<CUtlString> m_Vec;
		bool m_bAutoPurge;
	};

	ResourceGroupVector_t m_ResourceGroups;

	server_state_t m_State;
	int m_nRefCount;
	int m_Socket;
	int m_nTickCount;
	int m_nMinClientsLimit;
	int m_nMaxClientsLimit;
	int m_nPauseFlags;

	CGlobalVars m_globals;
	GameSessionConfiguration_t m_GameConfig;

	CUtlString m_szMapname;
	CUtlString m_szAddons;
	CUtlString m_szSkyname;
	CUtlString m_szUnk360;

	KeyValues* m_pLaunchOptions;

	INetworkStringTableContainer* m_pStringTables;
	INetworkStringTableContainer* m_pStringTables2;
	INetworkStringTable* m_pInstanceBaselineTable;
	INetworkStringTable* m_pLightStyleTable;
	INetworkStringTable* m_pUserInfoTable;

	CFrameSnapshotManager* m_pFrameSnapshotManager;
	// Serialized instance baselines by ServerClass::m_InstanceBaselineIndex, read by GetClassBaseline
	CAtomicMutex m_InstanceBaselinesMutex;
	CUtlHashMapImpl< int, intp, CUtlHashMapLargeDefEquals< int >, MurmurHash3Functor< int >, int, CVariableBitString > m_InstanceBaselines;

	bf_write m_Signon;
	CUtlLeanVector<byte> m_SignonBuffer;

	bool m_bPaused;
	CUtlClientVector m_Clients;
	CCompressedResourceManifest* m_pResourceManifest;

	CUtlLeanVector<byte> m_FullSendBuffer;	// Init capacity ~12.5 MB
	CUtlHash<CBaselineEntityData*> m_BaselineEntitiesMap;

	SpawnGroupHandle_t m_hActiveSpawnGroup;
	int m_nMaxClients;
	int m_nSpawnCount;
	int m_nServerClasses;
	int m_nServerClassBits;
	uint64 m_nCreateTime;

	bool m_bPendingChangeLevel;
	bool m_bHibernationRequested; // unverified: set by FinishChangeLevel, checked by ActivateServer
	bool m_bPreserveSteamID;
	bool m_bHibernating;

	CUtlVector<byte> m_GameData;

	CUtlLinkedList< CNetworkServerSpawnGroupCreatePrerequisites*> m_SpawnGroupPrerequisites;
	CUtlMap< SpawnGroupHandle_t, CNetworkServerSpawnGroup*  > m_SpawnGroups;

	CUtlVector<uint64> m_QueuedSpawnGroupUnloads; // SpawnGroupUnloadEntry_t (handle + flags)
	int m_bQueueSpawnGroupUnloads;
	// Spawn group created for the resources of the just in time manifests, unloaded when their count drops to 0
	SpawnGroupHandle_t m_hJustInTimeResourceSpawnGroup;

	CUtlVector<SplitDisconnect_t> m_QueuedForDisconnect;
	CUtlString m_GameType;
	int m_nGameDataVersion;
	CUtlVector<byte> m_GameData2;

	// Client whose command is being executed (Source 1 host_client), cleared by ServerEndSimulate; the kick command names it, or "Console" when it is unset
	CServerSideClientBase *m_pHostClient;
	IGameSpawnGroupMgr* m_pSpawnGroupMgr;
	// Told when the active spawn group changes
	void* m_pNotificationSink;
	// Set by SetGameSessionManifest
	HGameResourceManifest m_hGameSessionManifest;
	int m_nNotifyFlags;
	// The four pointers point into a reservation state owned by the server service (CHLTVServer keeps its own)
	// IsReserved() returns *m_pnReservationCookie != 0
	uint64* m_pnReservationCookie;
	// -1 while a client is connected, the time the server became empty otherwise
	float* m_pflTimeLastClientLeft;
	// Initialized to -1, no access found
	float* m_pflUnk968;
	float* m_pflReservationExpiryTime;
	// Incremented by OnSpawnGroupActivate ("CreationSequence=%d"), reset by SetGameSpawnGroupMgr
	int m_nSpawnGroupCreationSequence;
	// Last IResourceSystem::GetJustInTimeManifestCount() seen by the just in time spawn group update
	int m_nJustInTimeManifestCount;

	// handle = slot | (serial << 8). Init all 0xFFFF.
	uint16 m_nClientSlotSerial[ABSOLUTE_PLAYER_LIMIT];

	int m_nGameDataVersion2;
	int m_numGameSlots;
	float m_fLastCPUCheckTime;
	float m_fCPUPercent;
	float m_flTimescale;
	// Checked and cleared at the end of the simulation, which then refreshes the Steam game server data of every client
	bool m_bUnk1140;
	bool m_bIsMultiplayer;

	int m_nFrameTimeBucketCount;
	uint64 m_FrameTimeBuckets[11];
	int64 m_nFrameTimeSum;
	int64 m_nFrameTimeCount;

	int m_nComputeTimeBucketCount;
	uint64 m_ComputeTimeBuckets[61];
	double m_flComputeTimeSum;
	uint64 m_nComputeTimeCount;

	uint64 m_nReservationCookie;
	// Created with the "DemoAutoRecorder" fake client, records the demo straight from the game server
	CTvDirectDemoRecorder *m_pTvDirectDemoRecorder;
	// ICvar::GetTotalUserInfoCvarsByteSize(), refreshed by OnConVarCreated which then resizes every client's user info convar data
	uint64 m_nTotalUserInfoCvarsByteSize;
	int m_nUnk1760;
	void* m_pReplayDirector;
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
}; // sizeof 10088 (0x2768)


// Cached delta entity bit buffers of the HLTV server, one list per entity index
class CDeltaEntityCache
{
public:
	struct DeltaEntityEntry_s
	{
		DeltaEntityEntry_s *pNext;
		int nDeltaTick;
		int nBits;
	};

	int m_nTick;
	// Number of m_Cache entries in use, the destructor frees each of them
	int m_nMaxEntities;
	int m_nCacheSize;
	DeltaEntityEntry_s *m_Cache[ MAX_EDICTS ];
};

class CHLTVServer : public CNetworkGameServerBase, public IHLTVServer, public ITvStreamSink
{
public:
	// Only constructed and destroyed
	CUtlString m_szUnk1824;

	// Targets of the reservation pointers of the base class
	uint64 m_nHLTVReservationCookie;
	float m_flHLTVTimeLastClientLeft;
	float m_flHLTVUnk968;
	float m_flHLTVReservationExpiryTime;

	// Owns the broadcast and the demo recorder ("HLTVServerAsync" thread pool)
	CHLTVServerAsync *m_pAsync;
	// Client of the game server that feeds this master proxy, nullptr on a relay (IsMasterProxy)
	CServerSideClientBase *m_MasterClient;
	// Game server of m_MasterClient
	CNetworkGameServer *m_Server;

	// CHLTVClientState, the connection of a relay to its upstream server
#ifdef PLATFORM_WINDOWS
	uint8 m_ClientState[ 2888824 ];
#else
	uint8 m_ClientState[ 2888848 ];
#endif

	IHLTVDirector *m_Director;
	// Slot of the HLTV client on the game server (GetHLTVSlot)
	CPlayerSlot m_nPlayerSlot;
	CDeltaEntityCache m_DeltaCache;

	// Read by CHLTVClient::SendSnapshot
	bool m_bUnk3021832;
	// Set by StartRelay, cleared when the signon completes
	bool m_bSignonState;
	// Set by the start-up when either of two command line parameters is present; the retry command then reports "SourceTV in Primary-Only mode."
	bool m_bPrimaryOnlyMode;
	// Zeroed with the flags above, no access found
	uint8 m_unk3021835[ 3 ];

	// GetOnlineTime() returns the time since it
	float m_flStartTime;
	float m_flFPS;
	int m_nGameServerMaxClients;
	// The status is sent again 8 seconds after it
	float m_fNextSendUpdateTime;
	netadr_t m_RootServer;
	// GetGlobalStats()
	int m_nGlobalSlots;
	int m_nGlobalClients;
	int m_nGlobalProxies;
	// Zeroed by the constructor, no access found
	uint8 m_unk3021880[ 12 ];

	bf_write m_FullSendTables;
	CUtlLeanVector< byte > m_FullSendTablesBuffer;
	// Read by ServerPostSimulate
	float m_flUnk3021952;
	// Storage of m_BroadcastEventBuf, set up by StartMaster
	byte m_BroadcastEventBufData[ 98304 ];
	// Serializes the events of BroadcastHltvEvent
	bf_write m_BroadcastEventBuf;
	// SourceTV relay instance ("relay" / "non-relay")
	bool m_bRelay;
	// SourceTV[%d]
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
	virtual void	StartupServer( const GameSessionConfiguration_t &config, ISource2WorldSession *pWorldSession, const char * ) = 0;
	virtual void	SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;
	virtual void	AddServerPrerequisites( const GameSessionConfiguration_t &, const char *, ILoopModePrerequisiteRegistry *, bool ) = 0;
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
	virtual void	*GetServerSerializersMsg( void ) = 0;
	virtual IGameSpawnGroupMgr *GetGameSpawnGroupMgr( void ) = 0;
	virtual bool	IsSaveRestoreAllowed( CUtlString *pError ) = 0;
	virtual bool	IsEntityReportActive( void ) = 0;
	// Returns nullptr unless the entity report is active and targets this slot or every slot
	virtual IEntityReport *GetEntityReport( int nSlot ) = 0;
	// Empty in this build
	virtual void	unk046( void ) = 0;
	virtual void	unk047( void ) = 0;
	virtual bool	ThreadInPrimaryOrSecondaryMainThread( void ) = 0;
	virtual void	EnterSyncInterval( void ) = 0;
	// AMNOTE: The bool picks which of two parallel work modes clients enter
	virtual void	ExitSyncInterval( bool ) = 0;
};

typedef CNetworkGameServerBase IServer;


#endif // ISERVER_H
