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
#include <icvar.h>
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
struct EventServerSimulate_t;
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

	virtual void	ServerSimulate( const EventServerSimulate_t & ) = 0;
	virtual void	ServerPostSimulate( const EventServerPostSimulate_t & ) = 0;

	virtual void	LoadSpawnGroup( const SpawnGroupDesc_t & ) = 0;
	virtual void	AsyncUnloadSpawnGroup( unsigned int, /*ESpawnGroupUnloadOption*/ int ) = 0;
	virtual void	PrintSpawnGroupStatus( void ) const = 0;

	// returns the game time scale (multiplied in conjunction with host_timescale)
	virtual float	GetTimescale( void ) const = 0; 

	virtual bool	IsSaveRestoreAllowed( void ) const = 0;

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
	virtual bool	SpawnServer( const char * ) = 0;

	virtual int 	GetSpawnGroupLoadingStatus( SpawnGroupHandle_t ) = 0;
	virtual void	SetSpawnGroupDescription( SpawnGroupHandle_t, const char * ) = 0;

	virtual void*	StartChangeLevel( const char *pszMapName, const char *pszLandmark, const char *pszAddons ) = 0;
	virtual void	FinishChangeLevel( CServerChangelevelState * ) = 0;
	virtual bool	IsChangelevelPending( void ) const = 0;

	virtual void	GetAllLoadingSpawnGroups( CUtlVector<SpawnGroupHandle_t> *pOut ) = 0;

	virtual void	PreserveSteamID( void ) = 0;

	virtual const CSVCMsg_GameSessionConfiguration &GetGameConfig() = 0;

	virtual void	ReserveServerForQueuedGame( const char *pszReason ) = 0;

	virtual bool	IsReserved() = 0; // return m_nReservationCookie != 0
	virtual bool	IsMultiplayer() = 0; // return m_bIsMultiplayer
	virtual bool	IsPlayingSoloAgainstBots() = 0;

	virtual void	BroadcastPrintf( const char *pszFmt, ... ) FMTFUNCTION( 2, 3 ) = 0;

	virtual void	SetClientConnect( CPlayerSlot slot, bool b = true ) = 0;

	virtual void	NotifySceneViewDebugOverlays( void *pSceneView, bool bUnk ) {};

	virtual void	BroadcastMessage( INetworkMessageInternal *pSerializer, const CNetMessage *pNetMessage, const IRecipientFilter *pFilter ) = 0;
	virtual bool	IsRecordingDemo() = 0;

	virtual uint8	GetClientConnectionType( CPlayerSlot slot ) = 0;
};

class CNetworkGameServerBase : public INetworkGameServer, protected IConnectionlessPacketHandler, protected IConVarListener
{
public:
	virtual ~CNetworkGameServerBase() = 0;
	
	server_state_t	GetServerState() { return m_State; }

	virtual void	SetMaxClients( int nMaxClients ) = 0;
	virtual void	CreateClient( CPlayerSlot &nSlot, CSteamID nSteamID, const char *pszName ) = 0;

public: // IConnectionlessPacketHandler
	virtual bool	ProcessConnectionlessPacket( netpacket_t *packet ) = 0; // process a connectionless packet

public: // IConVarListener
	virtual void	OnConVarCreated( ConVarRefAbstract *pNewCvar ) = 0;
	virtual void	OnConCommandCreated( ConCommand *pNewCommand ) {};

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
	CUtlVector<CUtlString> m_vecUnk;
	char pad40[8];
	server_state_t m_State;
	int m_nRefCount;
	int m_Socket;
	int m_nTickCount;
	int m_nMinClientLimit;
	void (*UnkFunc)();
	CGlobalVars m_globals;
	GameSessionConfiguration_t m_GameConfig;
	CUtlString m_szMapname;
	CUtlString m_szAddons;
	CUtlString m_szSkyname;
	CUtlString m_szUnk344;
	KeyValues* m_KV352;
	CNetworkStringTableContainer* m_StringTables;
	CNetworkStringTableContainer* m_StringTables2;
	INetworkStringTable* m_pInstanceBaselineTable;
	INetworkStringTable* m_pLightStyleTable;
	INetworkStringTable* m_pUserInfoTable;
	INetworkStringTable* m_pServerStartupTable;
	CFrameSnapshotManager m_snapshotManager;
	char pad544[24];
	bf_write m_Signon;
	CUtlLeanVector<byte> m_SignonBuffer;
	bool m_bIsBackgroundMap;
	CUtlClientVector m_Clients;
	CCompressedResourceManifest* m_pResourceManifest;
#ifdef _WIN32
	char pad648[40];
#else
	char pad648[48];
#endif
	SpawnGroupHandle_t m_hActiveSpawnGroup;
	int m_nMaxClients;
	int m_nSpawnCount;
	int serverclasses;
	int serverclassbits;
	uint64 m_nCreateTime;
	bool m_bPendingChangeLevel;
	bool m_bUnk753;
	bool m_bPreserveSteamID;
	bool m_bHibernating;
	char pad732[116];
	CUtlVector<byte> m_GameData;
	CUtlString m_GameType;
	char pad880[16];
	CUtlVector<SplitDisconnect_t> m_QueuedForDisconnect;
	IGameSpawnGroupMgr* m_pSpawnGroupMgr2;
	ISource2WorldSession* m_pWorldSession;
	HGameResourceManifest m_pGameSessionManifest;
	SpawnGroupHandle_t m_hNextSpawnGroup;
	uint64* m_nReservationCookie;
	float* m_flTimeLastClientLeft;
	void* unk968;
	float* m_flReservationExpiryTime;
	char pad980[136];
	int m_GameDataVersion;
	int m_numGameSlots;
	float m_fLastCPUCheckTime;
	float m_fCPUPercent;
	float m_flTimescale;
	bool unk1140;
	bool m_bIsMultiplayer;
};

class CNetworkGameServer : public CNetworkGameServerBase {
public:
	CUtlString m_szStartspot;
	bf_write m_FullSendTables;
	CUtlLeanVector<byte> m_FullSendTablesBuffer;
	CPrecacheItem generic_precache[MAX_GENERIC];
	char pad9456[8];
	CPrecacheItem decal_precache[MAX_BASE_DECALS];
	char pad17656[8];
	INetworkStringTable* m_pGenericPrecacheTable;
	INetworkStringTable* m_pDecalPrecacheTable;
	CPureServerWhitelist* m_pPureServerWhitelist;
	bool allowsignonwrites;
}; // sizeof 17696

class CHLTVServer : public CNetworkGameServerBase {
public:

}; // sizeof 2824864

abstract_class INetworkServerService : public IEngineService
{
public:
	virtual ~INetworkServerService() {}
	virtual CNetworkGameServer	*GetNetworkServer( void ) = 0;
	CNetworkGameServerBase *GetIGameServer( void ) { return static_cast<CNetworkGameServerBase *>( GetNetworkServer() ); }
	virtual bool	IsActiveInGame( void ) const = 0;
	virtual bool	IsMultiplayer( void ) const = 0;
	virtual void	StartupServer( const GameSessionConfiguration_t &config, ISource2WorldSession *pWorldSession, const char * ) = 0;
	virtual void	SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;
	virtual void	AddServerPrerequisites( const GameSessionConfiguration_t &, const char *, ILoopModePrerequisiteRegistry *, bool ) = 0;
	//virtual void	SetServerSocket( int ) = 0;
	virtual bool	IsServerRunning( void ) const = 0;
	virtual void	DisconnectGameNow( ENetworkDisconnectionReason ) = 0;
	virtual void	PrintSpawnGroupStatus( void ) const = 0;
	virtual void	SetFinalSimulationTickThisFrame( int ) = 0;
	virtual void	*GetGameServer( void ) = 0;
	//virtual int		GetTickInterval( void ) const = 0;
	//virtual void	ProcessSocket( void ) = 0;
	virtual int		GetServerNetworkAddress( void ) = 0;
	virtual bool	GameLoadFailed( void ) const = 0;
	virtual void	SetGameLoadFailed( bool bFailed ) = 0;
	virtual void	SetGameLoadStarted( void ) = 0;
	virtual void	unk_18019F5B0( void ) = 0;
	virtual void	StartChangeLevel( void ) = 0;
	virtual void	PreserveSteamID( void ) = 0;
	virtual CRC32_t	GetServerSerializersCRC( void ) = 0;
	virtual void	*GetServerSerializersMsg( void ) = 0;
};

typedef CNetworkGameServerBase IServer;


#endif // ISERVER_H
