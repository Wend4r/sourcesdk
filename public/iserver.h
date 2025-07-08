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
	virtual void	SpawnServer( const char * ) = 0;

	virtual int 	GetSpawnGroupLoadingStatus( SpawnGroupHandle_t ) = 0;
	virtual void	SetSpawnGroupDescription( SpawnGroupHandle_t, const char * ) = 0;

	virtual void*	StartChangeLevel( const char *pszMapName, const char *pszLandmark, const char *pszAddons ) = 0;
	virtual void	FinishChangeLevel( CServerChangelevelState * ) = 0;
	virtual bool	IsChangelevelPending( void ) const = 0;

	virtual void	GetAllLoadingSpawnGroups( CUtlVector<SpawnGroupHandle_t> *pOut ) = 0;

	virtual void	PreserveSteamID( void ) = 0;

	virtual void	unk001() = 0;

	virtual void	ReserveServerForQueuedGame( const char *pszReason ) = 0;

	virtual void	unk101() = 0;
	virtual void	unk102() = 0;
	virtual void	unk103() = 0;

	virtual void	BroadcastPrintf( const char *pszFmt, ... ) FMTFUNCTION( 2, 3 ) = 0;

	virtual void	unk201() = 0;
	virtual void	unk202() = 0;
	
};

class CNetworkGameServerBase : public INetworkGameServer, protected IConnectionlessPacketHandler
{
public:
	virtual ~CNetworkGameServerBase() = 0;
	
	server_state_t	GetServerState() { return m_State; }

	virtual void	SetMaxClients( int nMaxClients ) = 0;

public: // IConnectionlessPacketHandler
	virtual bool	ProcessConnectionlessPacket( netpacket_t *packet ) = 0; // process a connectionless packet

private:
	virtual bool	unk301( const ns_address *addr, bf_read *bf ) = 0;

public:
	virtual CPlayerUserId GetPlayerUserId( CPlayerSlot slot ) = 0;
	virtual const char *GetPlayerNetworkIDString( CPlayerSlot slot ) = 0;
	
	// Returns udp port of this server instance
	virtual int		GetUDPPort() = 0;
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

	virtual bool	IsInPureServerMode() = 0;
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
	// @boeing666: offsets only for linux
	char pad16[16]; // 16
	IGameSpawnGroupMgr* m_pSpawnGroupMgr; // 32
	char pad40[8]; // 40
	server_state_t m_State; // 48
	int m_nRefCount; // 52
	int m_Socket; // 56
	int m_nTickCount; // 60
	void (*UnkFunc)(); // 64
	void (*UnkFunc2)(); // 72
	CGlobalVars m_globals; // 80
	GameSessionConfiguration_t m_GameConfig; // 200
	CUtlString m_szMapname; // 320
	CUtlString m_szAddons; // 328
	CUtlString m_szSkyname; // 336
	CUtlString m_szUnk344; // 344
	KeyValues* m_KV352; // 352
	CNetworkStringTableContainer* m_StringTables; // 360
	CNetworkStringTableContainer* m_StringTables2; // 368
	INetworkStringTable* m_pInstanceBaselineTable; // 376
	INetworkStringTable* m_pLightStyleTable; // 384
	INetworkStringTable* m_pUserInfoTable; // 392
	INetworkStringTable* m_pServerStartupTable; // 400
	CFrameSnapshotManager m_snapshotManager; // 408
	char pad544[30]; // 544
	bf_write m_Signon; // 576
	CUtlMemory<byte> m_SignonBuffer; // 616
	bool m_bIsBackgroundMap; // 632
	CUtlClientVector m_Clients; // 640
	char pad664[20]; // 664
	CUtlMemory<byte> m_unk688; // 688
	char pad704[16]; // 704
	SpawnGroupHandle_t m_hActiveSpawnGroup; // 720
	int m_nMaxClients; // 724
	int m_nSpawnCount; // 728
	int serverclasses; // 732
	int serverclassbits; // 736
	uint64 m_nCreateTime; // 744
	bool m_bPendingChangeLevel; // 752
	bool m_bUnk753; // 753
	bool m_bPreserveSteamID; // 754
	bool m_bHibernating; // 755
	CUtlVector<netadr_t*> m_unk760; // 760
	CUtlLinkedList<CNetworkServerSpawnGroupCreatePrerequisites *> m_unk784; // 784
	CUtlMap<uint32,void*> m_unk816; // 816 something with spawngroups
	CUtlMap<uint32,void*> m_unk848; // 848 something with spawngroups
	CUtlVector<char> m_GameData; // 880
	CUtlString m_GameType; // 904
	char pad912[8]; // 912
	CUtlVector<SplitDisconnect_t> m_QueuedForDisconnect; // 920
	CServerSideClientBase* m_pHostClient; // 944
	IGameSpawnGroupMgr* m_pSpawnGroupMgr2;
	void* m_unk960; // 960
	HGameResourceManifest m_pGameSessionManifest; // 968
	char pad976[8]; // 976
	uint64 m_nReservationCookie; // 984
	float m_flTimeLastClientLeft; // 992
	char pad996[12]; // 996
	float m_flReservationExpiryTime; // 1008
	char pad1012[140]; // 1012
	int m_nFinalSimulationTickThisFrame; // 1152
	int m_GameDataVersion; // 1156
	int m_numGameSlots; // 1160
	float m_fLastCPUCheckTime; // 1164
	float m_fCPUPercent; // 1168
	float m_flTimescale; // 1172
	bool m_bUnk1176; // 1176
	bool m_bUnk1177; // 1177
	uint64 m_nMatchId; // 1184
	void* m_pUnk1192; // 1192
};

class CNetworkGameServer : public CNetworkGameServerBase {
public:
	CUtlString m_szStartspot; // 1200
	bf_write m_FullSendTables; // 1208
	CUtlMemory<byte> m_FullSendTablesBuffer; // 1248
	CPrecacheItem generic_precache[MAX_GENERIC]; // 1264
	char pad9456[8]; // 9456
	CPrecacheItem decal_precache[MAX_BASE_DECALS]; // 9464
	char pad17656[8]; // 17656
	INetworkStringTable* m_pGenericPrecacheTable; // 17664
	INetworkStringTable* m_pDecalPrecacheTable; // 17672
	CPureServerWhitelist* m_pPureServerWhitelist; // 17680
	bool allowsignonwrites; // 17688
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
