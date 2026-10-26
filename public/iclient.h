//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client-side networking interfaces reversed from CS2 engine2.dll.
//          Mirrors iserver.h (INetworkGameServer / CNetworkGameServerBase).
//
//=============================================================================//

#ifndef ICLIENT_H
#define ICLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include <engine/IEngineService.h>
#include <eiface.h>
#include <engine2/inetworkclientservice.h>
#include <playerslot.h>
#include <playeruserid.h>
#include <ns_address.h>
#include <spawngrouptypes.h>
#include <resourcefile/resourcetype.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <tier0/utlbuffer.h>
#include <tier0/utlsignalslot.h>
#include <tier0/threadtools.h>
#include <tier1/bitbuf.h>
#include <tier0/bitvec.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlmap.h>
#include <tier1/utllinkedlist.h>
#include <utlhashmaplarge.h>
#include <globalvars_base.h>
#include <networksystem/inetchannel.h>
#include <inetmsghandler.h>
#include <iloopmode.h>
#include <engine/hoststate.h>
#include <engine/clientframe.h>
#include <engine/precache.h>
#include <qlimits.h>
#include <source2_steam_stats.pb.h>

class INetChannel;
class CNetworkGameClient;
class CClientChangelevelState;
class IGameSpawnGroupMgr;
class CCommand;
class CTransform;
class ClientClass;
class IEntityReport;
class CEntityReadInfo;
class bf_read;
class C2S_CONNECT_Message;
struct AudioState_t;
struct EventClientSimulate_t;
struct EventClientPollNetworking_t;
struct EventClientProcessNetworking_t;
struct EventClientProcessGameInput_t;
struct EventClientFrameSimulate_t;
struct EventClientAdvanceTick_t;
struct EventClientPostAdvanceTick_t;
class CNETMsg_Tick_t;
class CNETMsg_StringCmd_t;
class CNETMsg_SetConVar_t;
class CNETMsg_DebugOverlay_t;
class CNETMsg_SignonState_t;
class CNETMsg_SpawnGroup_Load_t;
class CNETMsg_SpawnGroup_ManifestUpdate_t;
class CNETMsg_SpawnGroup_Unload_t;
class CNETMsg_SpawnGroup_SetCreationTick_t;
class CSVCMsg_PeerList_t;
class CSVCMsg_VoiceInit_t;
class CSVCMsg_VoiceData_t;
class CSVCMsg_Print_t;
class CSVCMsg_ServerInfo_t;
class CSVCMsg_ClassInfo_t;
class CSVCMsg_FlattenedSerializer_t;
class CSVCMsg_Menu_t;
class CSVCMsg_GetCvarValue_t;
class CSVCMsg_SetPause_t;
class CSVCMsg_ClearAllStringTables_t;
class CSVCMsg_CreateStringTable_t;
class CSVCMsg_UpdateStringTable_t;
class CSVCMsg_SetView_t;
class CSVCMsg_PacketEntities_t;
class CSVCMsg_SplitScreen_t;
class CSVCMsg_CmdKeyValues_t;
class CSVCMsg_EncryptedData_t;
class CSVCMsg_RconServerDetails_t;
class CSVCMsg_Sounds_t;
class CSVCMsg_Prefetch_t;
class CSVCMsg_StopSound_t;
class CSVCMsg_PacketReliable_t;
class CSVCMsg_FullFrameSplit_t;
class CSVCMsg_HLTVStatus_t;
class CSVCMsg_ServerSteamID_t;
class CSVCMsg_HltvReplay_t;
class CSVCMsg_UserCommands_t;
class NetMessageConnectionClosed_t;
class NetMessageConnectionCrashed_t;
class NetMessageSplitscreenUserChanged_t;
class NetMessagePacketStart_t;
class NetMessagePacketEnd_t;
class CMsgPlayerInfo;
class CNetMessage;
class CNetworkClientSpawnGroup;
class CNetworkClientSpawnGroupCreatePrerequisites;
class INetworkStringTableContainer;
class ServerClass;
class CChangeFrameList;
class CNetworkGameClientBase;
struct SerializedEntityData_t;
struct EventServerPostSimulate_t;
struct EventServerBeginAsyncPostTickWork_t;

// cl_ticktiming stages, named after the lines of its detailed report
enum TickTimingStage_t
{
	TICK_TIMING_FIRST_INPUT_TO_NET_SEND = 0,
	TICK_TIMING_SERVER_RECV_MARGIN,
	TICK_TIMING_SERVER_SIM,
	// From the receive of a server tick to the start of its simulation
	TICK_TIMING_RECV_TO_SIMULATE,
	// Frame quantization hidden by interpolation
	TICK_TIMING_FRAME_QUANTIZATION,
	TICK_TIMING_CLIENT_RECV_MARGIN,
	TICK_TIMING_RENDER,
	TICK_TIMING_TOTAL,

	TICK_TIMING_COUNT
};

abstract_class IGameEventWatcher
{
public:
	virtual bool OnGameEventReceived( const CNetMessage *pData ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Engine-facing client interface. Implemented by CNetworkGameClientBase.
// Vtable indices are absolute (primary vtable of CNetworkGameClient, engine2.dll).
//-----------------------------------------------------------------------------
abstract_class INetworkGameClient
{
public:
	virtual void Init( int nSocket ) = 0;
	virtual void SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;

	// Returns &m_GlobalVars
	virtual CGlobalVarsBase *GetGlobals() = 0;

	virtual int GetClientTickCount() const = 0;
	virtual int GetServerTickCount() const = 0;
	// Tick of the last CSVCMsg_PacketEntities; 0 in CNetworkGameClientBase
	virtual int GetLastPacketEntitiesTick() = 0;
	// Tick of the last CNETMsg_Tick, logged as the "s" tick by cl_clockdbg
	virtual int GetServerTick() = 0;
	// last_cmd_number_executed of the last CSVCMsg_PacketEntities
	virtual int GetLastCommandNumberExecuted() = 0;
	// CNetworkGameClient::CommandHistoryEntry_t::m_bReceivedLate of the command, for commands up to the last executed one
	virtual bool WasCommandReceivedLate( int nCommandNumber ) = 0;
	virtual void SetFrameTime( float flFrameTime ) = 0;
	// nConnectionLocality holds CONNECTION_TYPE_* flags (serversideclientbase.h): CONNECTION_TYPE_LOCAL_MACHINE for a server in
	// another process on this machine, with CONNECTION_TYPE_SAME_PROCESS for a loopback or same-process server, and
	// CONNECTION_TYPE_REMOTE for a remote server
	virtual void Connect( CSplitScreenSlot nSlot, int nSocket, const ns_address &addr, HSteamNetConnection hConnection, uint8 nConnectionLocality ) = 0;
	virtual void Disconnect( ENetworkDisconnectionReason reason ) = 0;
	virtual bool IsActive() const = 0;
	virtual bool IsConnected() const = 0;
	virtual bool IsPaused() const = 0;
	// The local server is paused by PAUSE_GROUP_BUG ("setpause bug")
	virtual bool IsBugPaused() = 0;
	virtual bool IsInGame() const = 0;
	virtual bool IsBackgroundMap() const = 0;
	virtual int GetMaxClients() const = 0;
	virtual void Simulate( const EventClientSimulate_t &event ) = 0;
	virtual void ClientPollNetworking( const EventClientPollNetworking_t &event ) = 0;
	virtual void ClientProcessNetworking( const EventClientProcessNetworking_t &event ) = 0;
	virtual void OnClientProcessGameInput( const EventClientProcessGameInput_t &event ) = 0;
	virtual void OnClientFrameSimulate( const EventClientFrameSimulate_t &event ) = 0;
	virtual void OnClientAdvanceTick( const EventClientAdvanceTick_t &event ) = 0;
	virtual void OnClientPostAdvanceTick( const EventClientPostAdvanceTick_t &event ) = 0;
	// Run the game client's frame stage notifications 10 and 11
	virtual void OnServerPostSimulate( const EventServerPostSimulate_t &event ) = 0;
	virtual void OnServerBeginAsyncPostTickWork( const EventServerBeginAsyncPostTickWork_t &event ) = 0;
	// cl_ticktiming stage time in seconds, the last sample or its running average, clamped to -9.9 from below
	virtual float GetTickTiming( TickTimingStage_t nStage, bool bSmoothed ) = 0;
	virtual void RequestServerStatus( CSplitScreenSlot nSlot ) = 0;
	virtual void ForceFullUpdate( const char *pszReason ) = 0;
	virtual void PrintSpawnGroupStatus() const = 0;
	virtual bool GetCameraPosition( CSplitScreenSlot nSlot, CTransform *pTransform ) = 0;
	virtual CPlayerSlot GetLocalPlayer( CSplitScreenSlot nSlot ) const = 0;

	// MSVC places the CCommand overload first
	virtual void ServerCmd( int nCommandSrc, const char *pszCmd ) = 0;
	virtual void ServerCmd( int nCommandSrc, const CCommand &args ) = 0;

	virtual void SendStringCmd( CSplitScreenSlot nSlot, const char *pszCmd ) = 0;
	virtual void SplitScreenConnect( CSplitScreenSlot nSlot ) = 0;
	virtual int GetMaxSplitScreenPlayers() const = 0;
	virtual INetChannel *GetNetChannel( CSplitScreenSlot nSlot ) = 0;

	// Same implementation as GetNetChannel
	virtual INetChannel *unk042( CSplitScreenSlot nSlot ) = 0;

	// Empty in this build
	virtual void unk043() = 0;
	virtual void UpdateAudioState( AudioState_t *pAudioState, CSplitScreenSlot nSlot ) const = 0;
	virtual float GetTime() const = 0;
	// Fails when Steam is unavailable or no auth session ticket could be created for the game server
	virtual bool PrepareSteamConnectResponse( uint64 nGameServerSteamID, bool bGameServerSecure, const ns_address &addr, C2S_CONNECT_Message &msg ) = 0;
	virtual bool IsLowViolence() const = 0;
	virtual int GetHostVersion() const = 0;
	virtual float GetLastTimeStamp() const = 0;
	virtual int GetLastServerTick() const = 0;
	virtual void SetGameSessionManifests( HGameResourceManifest hManifest1, HGameResourceManifest hManifest2 ) = 0;
	virtual void SpewSerializers( const char *pszName ) = 0;
	virtual void EntityClassCompletion( const char *pszPartial, CUtlVector< CUtlString > &commands ) = 0;
	virtual bool IsSpawnGroupAsyncLoaded( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual ISource2WorldSession *GetWorldSession() = 0;
	virtual const char *GetSpawnGroupName( SpawnGroupHandle_t hSpawnGroup ) = 0;
	// Returns the state that FinishChangeLevel takes
	virtual CClientChangelevelState *StartChangeLevel() = 0;
	virtual void FinishChangeLevel( CClientChangelevelState *pState ) = 0;
	virtual bool IsChangelevelPending() const = 0;
	virtual void GetAllLoadingSpawnGroups( CUtlVector< SpawnGroupHandle_t > *pSpawnGroups ) = 0;
	virtual bool ProcessConnectionlessPacket( const ns_address &addr, bf_read &buf ) = 0;
	virtual void BlockDisconnectionTracking( bool bBlock ) = 0;
};

struct GameTickFraction_t;

// Connection of one split screen player; slot 0 is the main player
struct ClientSplitScreenPlayer_t
{
	INetChannel *m_pNetChannel;
	// Player slot from CSVCMsg_ServerInfo or CSVCMsg_SplitScreen
	CPlayerSlot m_nPlayerSlot;
	CSplitScreenSlot m_nSplitScreenSlot;
	bool m_bActive;
	// Added by CSVCMsg_SplitScreen; Disconnect removes it from the split screen manager
	bool m_bSplitScreenUser;
	// The nConnectionLocality of INetworkGameClient::Connect
	uint8 m_nConnectionLocality;
};

// Server class name and the client class it was matched to ("%d : %s matched to %s")
struct C_ServerClassInfo
{
	CUtlString m_ClassName;
	ClientClass *m_pClientClass;
};

// Connection attempt state that the connectionless packet handler and the connect packet use
struct ClientConnectState_t
{
	CNetworkGameClientBase *m_pClient;
	// Sender of the S2C_CONNECTION packet
	ns_address m_ResponseAddress;
	uint8 m_pad028[ 0x60 - 0x28 ];
	// Address of the server being connected to ("Connection to %s failed")
	ns_address m_ServerAddress;
	uint8 m_pad080[ 0x88 - 0x80 ];
	// Game server SteamID and secure flag passed to PrepareSteamConnectResponse
	uint64 m_nGameServerSteamID;
	// Match session to join instead of connecting ("joinsession")
	uint64 m_nMatchSessionID;
	uint32 m_nChallenge;
	// PROTOCOL_* value, PROTOCOL_STEAM for Steam authentication
	uint8 m_nAuthProtocol;
	bool m_bGameServerSecure;
	uint8 m_pad09E[ 0xA0 - 0x9E ];
	int m_nSocket;
	uint8 m_pad0A4[ 0xB0 - 0xA4 ];
	// Set by INetSupport::UpdateClientReservation
	uint64 m_nReservationCookie;
	uint8 m_pad0B8[ 0xC8 - 0xB8 ];
};

// Inline-or-heap list of 16-bit values: up to 12 in place, otherwise m_bExternal is set and the storage holds the pointer
struct SendProxyRecipientList_t
{
	union
	{
		uint16 m_Inline[ 12 ];
		uint16 *m_pElements;
	};
	int16 m_nCount;
	bool m_bExternal;
};

// Element of PackedEntity::m_Recipients; packed entities with equal lists and equal +0x48 values compare equal
class CSendProxyRecipients
{
public:
	uint8 m_pad000[ 0x08 ];
	SendProxyRecipientList_t m_Lists[ 2 ];
	uint8 m_pad048[ 0x58 - 0x48 ];
};

// Serialized state of an entity, here as an entity baseline
class PackedEntity
{
public:
	// Zero on the client
	ServerClass *m_pServerClass;
	// Class the baseline was decoded for; a lookup with another class finds no baseline
	ClientClass *m_pClientClass;
	// Pooled change list, released with the entity
	CChangeFrameList *m_pChangeFrameList;
	int m_nEntityIndex;
	uint8 m_pad01C[ 0x20 - 0x1C ];
	SerializedEntityData_t *m_pData;
	uint8 m_pad028[ 0x30 - 0x28 ];
	CUtlVector< CSendProxyRecipients > m_Recipients;
	uint8 m_pad048[ 0x58 - 0x48 ];
};

//-----------------------------------------------------------------------------
// Purpose: Concrete client base. Adds the network message Process* handlers,
// clock-drift / prediction internals and the sign-on state machine.
// Overrides of the secondary bases (message pre-filter, channel notify, game
// event watcher) and the destructor live only in their own vtables on Windows;
// the Itanium ABI also gives each of them a slot at its declaration here.
//-----------------------------------------------------------------------------
class CNetworkGameClientBase : public INetworkGameClient, public CUtlSlot, public INetworkMessageProcessingPreFilter, public IConnectionlessPacketHandler, public INetworkChannelNotify, public IGameEventWatcher
{
public:
	virtual void TransmitNetChannel() = 0;
	// Tick packet receive margin in seconds, recomputed from the cl_tickpacket convars
	virtual float GetTickPacketReceiveMargin() = 0;
	// Fraction of a tick that GetTime adds to the client tick
	virtual float GetTickFraction() = 0;
	// Forwards to ISource2Client::GetPredictionTickFraction
	virtual GameTickFraction_t GetPredictionTickFraction() = 0;
	// The pending connection uses PROTOCOL_STEAM
	virtual bool IsUsingSteamAuthentication() = 0;
	// Percentage of the outgoing (user commands) or incoming (server ticks) flow's ticks that were dropped or late in the last
	// window of more than 256 ticks
	virtual float GetOutgoingNetworkMissRate() = 0;
	virtual float GetIncomingNetworkMissRate() = 0;
	// The nConnectionLocality that Connect stored for the split screen slot
	virtual uint8 GetConnectionLocality( CSplitScreenSlot nSlot ) = 0;
	// The TrueViewLookAhead tick gobble or aggressive slowdown is running
	virtual bool IsLookAheadFixupActive() = 0;

	// Overrides the connectionless packet handler's virtual destructor: no slot of its own on Windows, two here on Linux
	virtual ~CNetworkGameClientBase() {}

	virtual void SetTickRemainder( float flTickRemainder ) = 0;
	// Copied to CGlobalVarsBase::m_flUsercmdTickFraction on each client frame simulate
	virtual void SetUsercmdTickFraction( float flFraction ) = 0;
	virtual void ApplyClockDrift( int nServerTick, int nClientTick ) = 0;
	// ISource2Client::GetLocalPlayerPawnHandle() is not -1
	virtual bool HasLocalPlayerPawn() = 0;
	// Resets the move message, then calls ISource2Client::CreateMove for each active split screen slot; skipped on background
	// maps and before SIGNONSTATE_FULL
	virtual void CreateMove() = 0;
	// ISource2Client::BeginNextTickMove for each active split screen slot (ApplyQueuedSubtickMoves on background maps), then
	// stamps the creation time of the latest command in the command history
	virtual void BeginNextTickMoves() = 0;
	// Sets m_GlobalVars.m_flUsercmdTickInterval from host_timescale, adjusted by the receive margin of the recent commands
	virtual void UpdateUsercmdTickInterval() = 0;
	// Cancels the scheduled asynchronous move send and, under the move send lock, records the sent move in the command
	// history, begins the next tick moves and updates the usercmd tick interval
	virtual void CompleteAsyncMoveSend() = 0;
	// CNetworkGameClient::m_flAsyncMoveSendDelay
	virtual float GetAsyncMoveSendDelay() = 0;
	// Logs "SentTickSync"
	virtual void SendTickSync() = 0;
	// Runs client side prediction ("RunPrediction") unless a demo is playing or prediction is busy
	virtual void RunPrediction() = 0;
	virtual void ClientPredictionPostNetupdate() = 0;
	// Records the render and present times of the commands in the server tick history and prints cl_ticktiming
	virtual void UpdateTickTiming() = 0;
	virtual bool IsHLTV() = 0;

	// Overrides of the secondary bases: no slots of their own on Windows, one each here on Linux
	virtual bool FilterMessage( const CNetMessage *pData, INetChannel *pChannel ) override = 0;
	// Clears the current split screen slot's channel when it is pChannel; bRequestIdle then also requests the idle host state
	virtual void OnShutdownChannel( INetChannel *pChannel, bool bRequestIdle ) override = 0;
	virtual bool OnGameEventReceived( const CNetMessage *pData ) override = 0;

	virtual bool GetPlayerInfo( CPlayerSlot nSlot, CMsgPlayerInfo &info ) = 0;
	virtual CPlayerUserId GetPlayerUserId( CPlayerSlot nSlot ) = 0;

	// Registers the message handlers below on the channel
	virtual void ConnectionStart( INetChannel *pNetChannel ) = 0;

public: // Message Handlers
	virtual bool ProcessTick( const CNETMsg_Tick_t &msg ) = 0;
	virtual bool ProcessStringCmd( const CNETMsg_StringCmd_t &msg ) = 0;
	virtual bool ProcessSetConVar( const CNETMsg_SetConVar_t &msg ) = 0;
	virtual bool ProcessDebugOverlay( const CNETMsg_DebugOverlay_t &msg ) = 0;
	virtual bool ProcessSignonState( const CNETMsg_SignonState_t &msg ) = 0;
	virtual bool ProcessSpawnGroup_Load( const CNETMsg_SpawnGroup_Load_t &msg ) = 0;
	virtual bool ProcessSpawnGroup_ManifestUpdate( const CNETMsg_SpawnGroup_ManifestUpdate_t &msg ) = 0;
	virtual bool ProcessSpawnGroup_Unload( const CNETMsg_SpawnGroup_Unload_t &msg ) = 0;
	virtual bool ProcessSpawnGroup_SetCreationTick( const CNETMsg_SpawnGroup_SetCreationTick_t &msg ) = 0;
	virtual bool ProcessPeerList( const CSVCMsg_PeerList_t &msg ) = 0;
	virtual bool ProcessVoiceInit( const CSVCMsg_VoiceInit_t &msg ) = 0;
	virtual bool ProcessVoiceData( const CSVCMsg_VoiceData_t &msg ) = 0;
	virtual bool ProcessPrint( const CSVCMsg_Print_t &msg ) = 0;
	virtual bool ProcessServerInfo( const CSVCMsg_ServerInfo_t &msg ) = 0;
	virtual bool ProcessClassInfo( const CSVCMsg_ClassInfo_t &msg ) = 0;
	virtual bool ProcessFlattenedSerializer( const CSVCMsg_FlattenedSerializer_t &msg ) = 0;
	virtual bool ProcessMenu( const CSVCMsg_Menu_t &msg ) = 0;
	virtual bool ProcessGetCvarValue( const CSVCMsg_GetCvarValue_t &msg ) = 0;
	virtual bool ProcessSetPause( const CSVCMsg_SetPause_t &msg ) = 0;
	virtual bool ProcessClearAllStringTables( const CSVCMsg_ClearAllStringTables_t &msg ) = 0;
	virtual bool ProcessCreateStringTable( const CSVCMsg_CreateStringTable_t &msg ) = 0;
	virtual bool ProcessUpdateStringTable( const CSVCMsg_UpdateStringTable_t &msg ) = 0;
	virtual bool ProcessSetView( const CSVCMsg_SetView_t &msg ) = 0;
	virtual bool ProcessPacketEntities( const CSVCMsg_PacketEntities_t &msg ) = 0;
	virtual bool ProcessSplitScreen( const CSVCMsg_SplitScreen_t &msg ) = 0;
	virtual bool ProcessCmdKeyValues( const CSVCMsg_CmdKeyValues_t &msg ) = 0;
	virtual bool ProcessEncryptedData( const CSVCMsg_EncryptedData_t &msg ) = 0;
	virtual bool ProcessRconServerDetails( const CSVCMsg_RconServerDetails_t &msg ) = 0;
	virtual bool ProcessSounds( const CSVCMsg_Sounds_t &msg ) = 0;
	virtual bool ProcessPrefetch( const CSVCMsg_Prefetch_t &msg ) = 0;
	virtual bool ProcessStopSound( const CSVCMsg_StopSound_t &msg ) = 0;
	virtual bool ProcessPacketReliable( const CSVCMsg_PacketReliable_t &msg ) = 0;
	virtual bool ProcessFullFrameSplit( const CSVCMsg_FullFrameSplit_t &msg ) = 0;
	virtual bool ProcessHLTVStatus( const CSVCMsg_HLTVStatus_t &msg ) = 0;
	virtual bool ProcessServerSteamID( const CSVCMsg_ServerSteamID_t &msg ) = 0;
	virtual bool ProcessConnectionClosed( const NetMessageConnectionClosed_t &msg ) = 0;
	virtual bool ProcessConnectionCrashed( const NetMessageConnectionCrashed_t &msg ) = 0;
	virtual bool ProcessChangeSplitscreenUser( const NetMessageSplitscreenUserChanged_t &msg ) = 0;
	virtual bool ProcessPacketStart( const NetMessagePacketStart_t &msg ) = 0;
	virtual bool ProcessPacketEnd( const NetMessagePacketEnd_t &msg ) = 0;
	virtual bool ProcessHltvReplay( const CSVCMsg_HltvReplay_t &msg ) = 0;
	virtual bool ProcessUserCommands( const CSVCMsg_UserCommands_t &msg ) = 0;

public:
	// Parses the entities of the message into a local entity read info
	virtual bool ParsePacketEntities( const CSVCMsg_PacketEntities_t &msg ) = 0;
	// bClearStringTables also drops the "genericprecache" and "userinfo" table pointers
	virtual void Clear( bool bClearStringTables ) = 0;
	virtual void FullConnect( const ns_address &addr ) = 0;
	virtual void SetSignonState( SignonState_t state, int nSpawnCount, const CNETMsg_SignonState_t *pMsg ) = 0;
	// Applies the rate convars to the net channel
	virtual void RunFrame() = 0;
	// Installs the engine callbacks of the string tables, then the game client's
	virtual void InstallStringTableCallback( bool bCallForExistingStrings ) = 0;
	// Looks up the engine string tables, then lets the game client install its callbacks
	virtual void HookClientStringTable( bool bCallForExistingStrings ) = 0;
	// Value of the "name" convar
	virtual const char *GetClientName( CSplitScreenSlot nSlot ) = 0;
	virtual void ProcessReadPacketEntitiesWorkItems( CEntityReadInfo &readInfo ) = 0;
	virtual void OnPreserveEntity( CEntityReadInfo &readInfo ) = 0;
	// nSerialNumber is the 17-bit serial and hSpawnGroup the spawn group of the enter PVS record
	virtual void CopyNewEntity( CEntityReadInfo &readInfo, ClientClass *pClientClass, int nSerialNumber, SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual void CopyExistingEntity( CEntityReadInfo &readInfo ) = 0;
	virtual void OnReceivedUncompressedPacket() = 0;
	// Tail call to Clear
	virtual void CL_ClearState( bool bClearStringTables ) = 0;

public:
	// Server-side and client-side world groups of the connection
	WorldGroupList_t m_WorldGroups;
	WorldGroupList_t m_ClientSideWorldGroups;
	CGlobalVarsBase m_GlobalVars;
	ClientSplitScreenPlayer_t m_SplitScreenPlayers[ 4 ];
	// Addresses of the last connection; tv_retry reconnects a SourceTV relay to the first one
	CUtlVector< ns_address > m_RetryAddresses;
	ClientConnectState_t *m_pConnectState;
	// Indexed by the server class index of the entity records
	CUtlVector< C_ServerClassInfo > m_ServerClasses;
	// Client classes built from the local server's classes for replay ("ConstructFakeClientClassesListFromServerClasses")
	CUtlVector< ClientClass * > m_FakeClientClasses;
	// Entities of fake client classes that CopyNewEntity skipped creating for replay, by entity index
	CUtlHashtable< int, ClientClass * > m_FakeClassEntities;
	CSVCMsg_FlattenedSerializer_t *m_pLastFlattenedSerializer; // Copy of the last flattened serializer message
	// The client string table container "CL" until the server's tables are created
	INetworkStringTableContainer *m_pStringTableContainer;
	// Contents of cl_sendtable_cache_filename, read and written when cl_cache_sendtable is set
	CUtlBuffer m_SendTableCacheBuffer;
	// "maps/<map>.vpk" and the map name of CSVCMsg_ServerInfo
	CUtlString m_sLevelFileName;
	CUtlString m_sLevelName;
	// Set from CSVCMsg_ServerSteamID, PrepareSteamConnectResponse and INetSupport::UpdateClientReservation
	uint64 m_nServerSteamID;
	// Highest spawn group creation sequence of the loaded spawn groups
	int m_nMaxSpawnGroupCreationSequence;
	// Index into m_SplitScreenPlayers, set by CNETMsg_SplitscreenUserChanged
	int m_nSplitScreenSlot;
	SignonState_t m_nSignonState;
	// CRC of the server's serializers, compared with the send table cache
	uint32 m_nServerSerializersCRC;
	int m_nRefCount;
	// Socket of the net channels and of the connectionless packet polling
	int m_nSocket;
	// CSVCMsg_ServerInfo max_clients, server_count and protocol
	int m_nMaxClients;
	int m_nServerCount;
	int m_nServerProtocol;
	int m_nDeltaTick;
	// Net time that Connect stored; the HLTV relay client throttles its commands with it
	float m_flNextCmdTime;
	// Command queue state of the last packet from the server, see CNetworkClientService::m_CommandQueueInfoHistory
	ClientCommandQueueInfo_t m_CommandQueueInfo;
	// Bits of a server class index, from CSVCMsg_ServerInfo max_classes
	int m_nServerClassBits;
	// Spawn count of the SIGNONSTATE_SPAWN message that DeferActivate held back, -1 when none
	int m_nDeferredSpawnCount;
	ENetworkDisconnectionReason m_nDisconnectReason;
	// Set for CHLTVClientState, the connection of a SourceTV relay to its master; the game client's globals are not used
	bool m_bIsHLTVRelay;
	// CSVCMsg_SetPause paused
	bool m_bPaused;
	// Never set in libengine2; when set, the network quality and clock bookkeeping is skipped and the command queue
	// messages are logged with a lower severity
	bool m_bSkipNetworkQualityTracking;
	GameSessionConfiguration_t m_GameSessionConfiguration;
	CAtomicMutex m_BaselineHandlesMutex;
	// Serialized instance baselines by instance baseline index ("m_BaselineHandles.Find(%d) failed")
	CUtlHashMapImpl< int, SerializedEntityData_t *, CUtlHashMapLargeDefEquals< int >, MurmurHash3Functor< int >, int, CVariableBitString > m_BaselineHandles;
	int m_nClientTick;
	int m_nServerTick;
	ISource2WorldSession *m_pWorldSession;
	IGameSpawnGroupMgr *m_pSpawnGroupMgr;
	CUtlMap< SpawnGroupHandle_t, CNetworkClientSpawnGroup * > m_SpawnGroups;
	// Defaults to "unnamed"; only carried across a changelevel
	CUtlString m_unk3E0;
	CUtlLinkedList< CNetworkClientSpawnGroupCreatePrerequisites * > m_SpawnGroupPrerequisites;
	// The level name copied before Clear resets it; never read
	CUtlString m_unk400;
	// The two entity baseline sets that CopyEntityBaseline copies between
	CUtlVectorFixed< PackedEntity, MAX_EDICTS > m_EntityBaselines[ 2 ];
	// Entities with a baseline in each set
	CBitVec< MAX_EDICTS > m_EntityBaselineBits[ 2 ];
	// Destination set of the running CopyEntityBaseline
	int m_nCopyEntityBaselineTo;
	CAtomicMutex m_EntityBaselineBitsMutex;
	HGameResourceManifest m_hGameSessionManifests[ 2 ];
	// Next handle of a client-side spawn group, counting up from 0x80000000
	uint32 m_nNextClientSpawnGroupHandle;
	SpawnGroupHandle_t m_hActiveSpawnGroup;
	int m_nMaxSplitScreenPlayers;
	// Tick of the last CSVCMsg_PacketReliable that finished a reliable state
	int m_nLastReliablePacketTick;
	// The first total tick count of EventClientAdvanceTick_t; the ticks since then make up m_GlobalVars.m_nClientTickCount
	int m_nFirstTotalTicks;
	// SetRestrictServerCommands; set by default for the mods the constructor lists
	bool m_bRestrictServerCommands;
	// Set when CSVCMsg_ServerInfo has been processed, cleared by Clear; never read
	bool m_bUnk2C1455;
	bool m_bChangelevelPending;
	bool m_bIsBackgroundMap;
	// DeferActivate held back SIGNONSTATE_SPAWN until ResumeActivate
	bool m_bDeferredActivate;
	// Write the send table cache after the next flattened serializer
	bool m_bWriteSendTableCache;
	// ForceDemoRecordingFullUpdateAfterNextDeltaPacket
	bool m_bForceDemoRecordingFullUpdate;
	// CSVCMsg_HltvReplay delay
	int m_nHltvReplayDelay;
	// The "Disconnection during connection phase" warning was logged
	bool m_bReportedConnectionPhaseDisconnect;
	bool m_bBlockDisconnectionTracking;
};

// Reservoir of up to N samples: further samples randomly replace stored ones; m_nTotalCount counts every sample offered
template < typename T, int N >
struct NetworkFlowSamples_t
{
	int m_nCount;
	int m_nTotalCount;
	// Samples were added since the last sort
	bool m_bUnsorted;
	T m_Samples[ N ];
	T m_Max;
	T m_Min;
};

// Delivery outcome of a tick, counted into CMsgSource2NetworkFlowQuality ticks_good ... ticks_bad_other
enum NetworkTickOutcome_t
{
	NETWORK_TICK_GOOD = 0,
	NETWORK_TICK_GOOD_ALMOST_LATE,
	NETWORK_TICK_BAD_DROPPED,
	NETWORK_TICK_BAD_LATE,
	NETWORK_TICK_BAD_OTHER,
	// Dropped or late, but before the client needed the tick ("Hidden by %s")
	NETWORK_TICK_FIXED_DROPPED,
	NETWORK_TICK_FIXED_LATE,

	NETWORK_TICK_OUTCOME_COUNT
};

// Tick delivery statistics of one network flow, reported through CMsgSource2NetworkFlowQuality
struct NetworkFlowQualityStats_t
{
	// Ticks per NetworkTickOutcome_t since the last miss rate sample (reset after more than 256 ticks), then since the
	// connection started
	int m_nTickCounts[ NETWORK_TICK_OUTCOME_COUNT ];
	int m_nTotalTickCounts[ NETWORK_TICK_OUTCOME_COUNT ];
	// Percentage of the ticks of the last sample window that were dropped or late ("%.1f%% %s missed due to network")
	float m_flNetworkMissRate;
	// Sample windows without network misses, and without any misses
	int m_nPerfectNetSamples;
	int m_nPerfectSamples;
	// Miss rates of the sample windows in tenths of a percent (tick_missratenet_p*_x10)
	NetworkFlowSamples_t< uint8, 100 > m_MissRateSamples;
	// Receive margins in milliseconds (recvmargin_p*)
	NetworkFlowSamples_t< int16, 500 > m_RecvMarginSamples;
};

// ServerTickHistoryEntry_t::m_nFlags
#define SERVER_TICK_FLAG_EXPECTED_BEFORE_RECEIVE	( 1 << 0 )	// The look-ahead window reached the tick before it was received
#define SERVER_TICK_FLAG_NEEDED_BEFORE_RECEIVE		( 1 << 1 )	// "Server tick %d was not available when we needed it"
#define SERVER_TICK_FLAG_FRAME_TIME_AMNESTY			( 1 << 2 )	// A frame time amnesty was active, late ticks are not reported
#define SERVER_TICK_FLAG_OUT_OF_ORDER				( 1 << 3 )	// The net channel reported the frame as delivered out of order
#define SERVER_TICK_FLAG_JITTER_LATE				( 1 << 4 )	// The frame jitter exceeded the late threshold
#define SERVER_TICK_FLAG_BELOW_DESIRED_RECV_MARGIN	( 1 << 5 )	// Arrived below cl_clock_recvmargin_desired

// Receive record of one server tick
struct ServerTickHistoryEntry_t
{
	int m_nTick;
	// In-sequence number of the tick's net message
	int m_nSequenceNr;
	// Jitter INetChannelInfo::GetFrameStatus reported for the frame
	float m_flFrameJitter;
	// CNETMsg_Tick host_computationtime in seconds
	float m_flHostComputationTime;
	// SERVER_TICK_FLAG_*
	uint8 m_nFlags;
	// Plat_FloatTime() of the receive, of the start of its simulation, of its render and of its present
	double m_flReceiveTime;
	double m_flSimulateTime;
	double m_flRenderTime;
	double m_flPresentTime;
};

struct ReadPacketEntitiesWorkItem_t;

// Passes over the read packet entities work items
enum ReadPacketEntitiesPass_t
{
	// Reads the fields of each item into its serialized entity
	READ_PACKET_ENTITIES_PASS_READ_FIELDS = 0,
	// Applies the serialized entities to the entities
	READ_PACKET_ENTITIES_PASS_APPLY,
	// Both in one pass
	READ_PACKET_ENTITIES_PASS_READ_AND_APPLY,
};

// Shared state of the parallel passes over the read packet entities work items
struct ReadPacketEntitiesWorkState_t
{
	// Set before each pass
	ReadPacketEntitiesPass_t m_nPass;
	CNetworkGameClient *m_pClient;
	// Next item to process, taken atomically, and the end of the items
	ReadPacketEntitiesWorkItem_t *m_pNextItem;
	ReadPacketEntitiesWorkItem_t *m_pEndItem;
	// Serialized entities to free after the passes
	CUtlVector< SerializedEntityData_t * > m_SerializedEntities;
};

// The most-derived client class. Overrides a handful of base virtuals and
// adds no new slots on Windows.
class CNetworkGameClient : public CNetworkGameClientBase, public ISwitchLoopModeStatusNotify
{
public:
	// Send and receive record of one user command, indexed by the low byte of its number
	struct CommandHistoryEntry_t
	{
		int m_nCommandNumber;
		// ISource2Client::GetCommandTime of the command
		double m_flCommandTime;
		// Plat_FloatTime() when BeginNextTickMoves or DuplicateCommandToCatchUp created the command, 0 until then
		double m_flCreateTime;
		// Copies of m_flLastMoveSendTime and m_nLastMoveSequenceNr taken when the entry is filled
		double m_flSendTime;
		// Copy of m_flLastMoveSendStatus; the receive margin estimate skips entries below 0
		float m_flSendStatus;
		int m_nSendSequenceNr;
		// Receive margin the server reported for the command ("User command %d recv on server ..."), FLT_MAX until then
		float m_flServerRecvMargin;
		// Tick of the CSVCMsg_PacketEntities that first reported the command as executed
		int m_nExecutedTick;
		// Set when the server reported a negative receive margin for the command ("LATE")
		bool m_bReceivedLate;
	};

	// Last slot of the Linux primary vtable; on Windows only in the notify vtable
	virtual void OnSwitchLoopModeFinished( const char *pszLoopModeName, uint32 nRequestID, bool bSuccess ) override = 0;

public:
	CClientFrameManager m_FrameManager;
	// Entries of the "genericprecache" string table
	int m_nMaxPrecacheEntries;
	CPrecacheItem m_GenericPrecache[ MAX_GENERIC ];
	// Indexed by player slot (GetPlayerInfo, GetPlayerUserId)
	CUtlVector< CMsgPlayerInfo > m_PlayerInfos;
	INetworkStringTable *m_pGenericPrecacheTable;
	INetworkStringTable *m_pInstanceBaselineTable;
	INetworkStringTable *m_pUserInfoTable;
	INetworkStringTable *m_pServerQueryInfoTable;
	ReadPacketEntitiesWorkState_t *m_pReadPacketEntitiesWorkState;
	// Set from CSVCMsg_ServerInfo, logged as "ishltv"
	bool m_bIsHLTV;
	// The "frame window too big" warning of ProcessPacketEntities is active
	bool m_bFrameWindowTooBig;
	// ClientProcessNetworking handles incoming packets; cleared while the level load loop mode runs
	bool m_bProcessIncomingPackets;
	float m_flTickRemainder;
	float m_flUsercmdTickFraction;
	float m_flFrameTime;
	// Last CCLCMsg_LoadingProgress progress sent
	int m_nLastLoadingProgress;
	// First server tick of the connection; m_GlobalVars times are relative to it
	int m_nFirstServerTick;
	// Tick of the last CSVCMsg_PacketEntities
	int m_nLastPacketEntitiesTick;
	// Highest tick in m_ServerTickHistory
	int m_nHighestReceivedServerTick;
	int m_nLastCommandNumberExecuted;
	// m_nLastCommandNumberExecuted plus last_cmd_number_recv_delta of the last CSVCMsg_PacketEntities
	int m_nLastCommandNumberReceived;
	// Command number from which the server re-synchronized the command queue ("CQ: Server set re-sync at cmd %d")
	int m_nCommandQueueResyncCommand;
	// Parameters of the last CSVCMsg_HltvReplay
	int m_nHltvReplayDelay;
	int m_nHltvReplayPrimaryTarget;
	int m_nHltvReplayStopAt;
	int m_nHltvReplaySlowdownBegin;
	int m_nHltvReplaySlowdownEnd;
	float m_flHltvReplaySlowdownRate;
	// CNETMsg_Tick expected_long_tick and its reason ("Server tick %d was long (%.1fms) due to '%s'")
	int m_nExpectedLongTick;
	CUtlString m_sExpectedLongTickReason;
	CommandHistoryEntry_t m_CommandHistory[ 256 ];
	// Only zeroed by the constructor
	int m_nUnk2C6DB8;
	// Last command number whose delivery outcome was counted into m_NetworkFlowQualityStats
	int m_nLastEvaluatedCommandNumber;
	// Indexed by flow: FLOW_OUTGOING (user commands), FLOW_INCOMING (server ticks)
	NetworkFlowQualityStats_t m_NetworkFlowQualityStats[ 2 ];
	// Receive margin below which a command the server reports is logged as "later than desired"
	float m_flDesiredRecvMargin;
	// Receive record of one server tick, indexed by the low byte of the tick
	ServerTickHistoryEntry_t m_ServerTickHistory[ 256 ];
	// Last server tick whose delivery outcome was counted into m_NetworkFlowQualityStats
	int m_nLastEvaluatedServerTick;
	// TrueViewLookAhead state: excess margin, a change marker, the look-ahead in commands and in ticks, and the fixup
	// (-1 for the tick gobble, a positive count while the aggressive slowdown runs)
	float m_flLookAheadExcess;
	bool m_bLookAheadChanged;
	int m_nLookAheadCommands;
	float m_flLookAheadTicks;
	int m_nLookAheadFixup;
	// cl_ticktiming stages, the last sample and its running average each; read by GetTickTiming
	float m_flTickTimings[ TICK_TIMING_COUNT ][ 2 ];
	// Buffer of m_MoveMessage, the datagram SendMove transmits ("ClientSendMove")
	uint8 m_MoveMessageData[ 0x2000 ];
	bf_write m_MoveMessage;
	// Range of the user commands written to m_MoveMessage ("SentTickAsync   cmds %d...%d")
	int m_nMoveFirstCommandNumber;
	int m_nMoveLastCommandNumber;
	// Scheduled function of the asynchronous move send, -1 when none
	int m_nAsyncMoveSendFunction;
	// Time the asynchronous move send was requested; the send callback warns when it runs more than 10 ms later
	double m_flAsyncMoveSendRequestTime;
	CAtomicMutex m_AsyncMoveSendMutex;
	// Incoming net messages held back by cl_buffer_incoming_net_messages
	CUtlVector< CNetMessage * > m_BufferedNetMessages;
	// Sequence number and Plat_FloatTime() of the last move datagram, -1 when it could not be sent
	int m_nLastMoveSequenceNr;
	double m_flLastMoveSendTime;
	// 0 when SendMove transmitted the move, -1 after the history has taken it
	float m_flLastMoveSendStatus;
	// Lateness of the asynchronous move send, raised to new peaks (up to 10 ms) and decayed on every send
	float m_flAsyncMoveSendDelay;
	// Plat_FloatTime() seconds of the last VProf lite report and of the last network quality report sent to the server
	int m_nLastVProfReportTime;
	int m_nLastNetworkQualityReportTime;
	CMsgSource2NetworkFlowQuality m_NetworkFlowQuality[ 2 ];
	// Highest and current demo packet index read while peeking ahead in a demo
	int64 m_nHighestDemoPacketIndex;
	int64 m_nCurrentDemoPacketIndex;
};

#endif // ICLIENT_H
