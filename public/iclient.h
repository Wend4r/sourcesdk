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
#include <globalvars_base.h>
#include <networksystem/inetchannel.h>
#include <inetmsghandler.h>
#include <iloopmode.h>
#include <engine/hoststate.h>
#include <engine/clientframe.h>
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
struct EventServerPostSimulate_t;
struct EventServerBeginAsyncPostTickWork_t;

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
	// nConnectionLocality is 2 for a server in another process on this machine, 3 for a loopback or same-process server
	// and 4 for a remote server
	virtual void Connect( CSplitScreenSlot nSlot, int nSocket, const ns_address &addr, HSteamNetConnection hConnection, uint8 nConnectionLocality ) = 0;
	virtual void Disconnect( ENetworkDisconnectionReason reason ) = 0;
	virtual bool IsActive() const = 0;
	virtual bool IsConnected() const = 0;
	virtual bool IsPaused() const = 0;
	// The local server is paused by pause group 3 ("setpause bug")
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
	virtual float GetTickTiming( int nTiming, bool bSmoothed ) = 0;
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
	// The pending connection uses the Steam authentication protocol (3)
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
	uint8 m_unk0F0[ 0x60 ];
	CUtlVector< void * > m_unk150;
	void *m_unk168;
	uint8 m_unk170[ 0x30 ];
	uint8 m_unk1A0[ 0x20 ];
	CSVCMsg_FlattenedSerializer_t *m_pLastFlattenedSerializer; // Copy of the last flattened serializer message
	void *m_unk1C8;
	CUtlBuffer m_unk1D0;
	CUtlString m_unk220;
	CUtlString m_unk228;
	void *m_unk230;
	void *m_unk238;
	SignonState_t m_nSignonState;
	int m_nUnk244;
	int m_nRefCount;
	// Socket of the net channels and of the connectionless packet polling
	int m_nSocket;
	int m_unk250[ 3 ];
	int m_nDeltaTick;
	int m_nUnk260;
	// Command queue state of the last packet from the server, see CNetworkClientService::m_CommandQueueInfoHistory
	ClientCommandQueueInfo_t m_CommandQueueInfo;
	int m_unk2A0[ 3 ];
	bool m_bUnk2AC;
	bool m_bUnk2AD;
	bool m_bUnk2AE;
	GameSessionConfiguration_t m_GameSessionConfiguration;
	CAtomicMutex m_unk348;
	uint64 m_unk358[ 10 ];
	int m_nClientTick;
	int m_nServerTick;
	ISource2WorldSession *m_pWorldSession;
	IGameSpawnGroupMgr *m_pSpawnGroupMgr;
	bool m_bUnk3C0;
	uint64 m_unk3C8[ 3 ];
	CUtlString m_unk3E0;
	uint64 m_unk3E8[ 3 ];
	CUtlString m_unk400;
	uint64 m_unk408[ 2 ][ 0x160008 / 8 ];
	CBitVec< MAX_EDICTS > m_unk2C0418[ 2 ];
	int m_nUnk2C1418;
	CAtomicMutex m_unk2C1420;
	HGameResourceManifest m_hGameSessionManifests[ 2 ];
	int m_nUnk2C1440;
	int m_nUnk2C1444;
	int m_nMaxSplitScreenPlayers;
	int m_nUnk2C144C;
	int m_nUnk2C1450;
	bool m_bUnk2C1454;
	bool m_bUnk2C1455;
	bool m_bChangelevelPending;
	bool m_bIsBackgroundMap;
	bool m_bUnk2C1458;
	bool m_bUnk2C1459;
	bool m_bUnk2C145A;
	int m_nUnk2C145C;
	bool m_bUnk2C1460;
	bool m_bBlockDisconnectionTracking;
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
		double m_flUnk10;
		// Copies of m_flLastMoveSendTime and m_nLastMoveSequenceNr taken when the entry is filled
		double m_flSendTime;
		float m_flUnk20;
		int m_nSendSequenceNr;
		// Receive margin the server reported for the command ("User command %d recv on server ..."), FLT_MAX until then
		float m_flServerRecvMargin;
		int m_nUnk2C;
		// Set when the server reported a negative receive margin for the command ("LATE")
		bool m_bReceivedLate;
	};

	// Last slot of the Linux primary vtable; on Windows only in the notify vtable
	virtual void OnSwitchLoopModeFinished( const char *pszLoopModeName, uint32 nRequestID, bool bSuccess ) override = 0;

public:
	CClientFrameManager m_FrameManager;
	uint64 m_unk2C1520[ 0x2008 / 8 ];
	// Indexed by player slot (GetPlayerInfo, GetPlayerUserId)
	CUtlVector< CMsgPlayerInfo > m_PlayerInfos;
	uint64 m_unk2C3540[ 5 ];
	// Set from CSVCMsg_ServerInfo, logged as "ishltv"
	bool m_bIsHLTV;
	bool m_bUnk2C3569;
	bool m_bUnk2C356A;
	float m_flTickRemainder;
	float m_flUsercmdTickFraction;
	float m_flFrameTime;
	int m_unk2C3578[ 4 ];
	int m_nLastCommandNumberExecuted;
	// m_nLastCommandNumberExecuted plus last_cmd_number_recv_delta of the last CSVCMsg_PacketEntities
	int m_nLastCommandNumberReceived;
	int m_unk2C3590[ 8 ];
	CUtlString m_unk2C35B0;
	CommandHistoryEntry_t m_CommandHistory[ 256 ];
	void *m_unk2C6DB8;
	uint8 m_unk2C6DC0[ 2 ][ 1196 ];
	// Receive margin below which a command the server reports is logged as "later than desired"
	float m_flDesiredRecvMargin;
	uint8 m_unk2C771C[ 0x385C ];
	// Buffer of m_MoveMessage, the datagram SendMove transmits ("ClientSendMove")
	uint8 m_MoveMessageData[ 0x2000 ];
	bf_write m_MoveMessage;
	// Range of the user commands written to m_MoveMessage ("SentTickAsync   cmds %d...%d")
	int m_nMoveFirstCommandNumber;
	int m_nMoveLastCommandNumber;
	int m_nUnk2CCFA8;
	// Time the asynchronous move send was requested; the send callback warns when it runs more than 10 ms later
	double m_flAsyncMoveSendRequestTime;
	CAtomicMutex m_AsyncMoveSendMutex;
	uint64 m_unk2CCFC8[ 3 ];
	// Sequence number and Plat_FloatTime() of the last move datagram, -1 when it could not be sent
	int m_nLastMoveSequenceNr;
	double m_flLastMoveSendTime;
	float m_flUnk2CCFF0;
	// Lateness of the asynchronous move send, raised to new peaks (up to 10 ms) and decayed on every send
	float m_flAsyncMoveSendDelay;
	void *m_unk2CCFF8;
	CMsgSource2NetworkFlowQuality m_NetworkFlowQuality[ 2 ];
	void *m_unk2CD260[ 2 ];
};

#endif // ICLIENT_H
