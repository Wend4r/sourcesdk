//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Client-side network game classes of engine2 (CNetworkGameClientBase and the
//          interfaces it implements). Mirrors iserver.h.
//
//=============================================================================//

#ifndef ICLIENT_H
#define ICLIENT_H
#ifdef _WIN32
#pragma once
#endif

#include <engine/IEngineService.h>
#include <engine/hoststate.h>
#include <engine/precache.h>
#include <engine/clientframe.h>
#include <engine2/inetworkclientservice.h>
#include <networkbasetypes.pb.h>
#include <source2_steam_stats.pb.h>
#include <globalvars_base.h>
#include <iloopmode.h>
#include <inetmsghandler.h>
#include <playerslot.h>
#include <playeruserid.h>
#include <ns_address.h>
#include <spawngrouptypes.h>
#include <resourcefile/resourcetype.h>
#include <entity2/entityidentity.h>
#include <tier0/bitvec.h>
#include <tier0/threadtools.h>
#include <tier0/utlbuffer.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlstringtoken.h>
#include <tier1/utlhashmaplarge.h>
#include <tier1/utllinkedlist.h>
#include <tier1/utlmap.h>
#include <tier0/utlstring.h>
#include <tier1/utlvector.h>

class INetChannel;
class INetChannelInfo;
class INetworkMessageInternal;
class CNetMessage;
class CCommand;
class CTransform;
class CClientChangelevelState;
class IGameSpawnGroupMgr;
class ISource2WorldSession;
class CNetworkStringTableContainer;
class IFlattenedSerializers;
class CBaselineEntityData;
class ClientClass;
class IEntityReport;
class CEntityReadInfo;
class KeyValues;
class C2S_CONNECT_Message;
class CHLTVServer;
class CClientFrame;
class CFrameSnapshotManager;
struct AudioState_t;
struct EventClientSimulate_t;
struct EventClientPollNetworking_t;
struct EventClientProcessNetworking_t;
struct EventClientSendInput_t;
struct EventClientFrameSimulate_t;
struct EventClientAdvanceTick_t;
struct EventClientPostAdvanceTick_t;
struct EventClientOutput_t;
struct EventClientPostOutput_t;
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
class CSVCMsg_Broadcast_Command_t;
class CSVCMsg_UserCommands_t;
class CSVCMsg_UserMessage_t;
class NetMessageConnectionClosed_t;
class NetMessageConnectionCrashed_t;
class NetMessageSplitscreenUserChanged_t;
class NetMessagePacketStart_t;
class NetMessagePacketEnd_t;
namespace google { namespace protobuf { class Message; } }

//-----------------------------------------------------------------------------
// Purpose: Resource groups ("default", "engine", ...) a network game client or server keeps
// loaded, counted by the systems that asked for them.
//-----------------------------------------------------------------------------
struct ResourceGroupRef_t
{
	CUtlStringToken m_Token;
	CUtlString m_Name;
	int m_nRefCount;
};

struct ResourceGroupVector_t
{
	CUtlVector< ResourceGroupRef_t > m_Vec;
	// Named by what it does: when set, the index handed to the spawn group manager carries the high bit
	bool m_bHandleHighBit;
};

//-----------------------------------------------------------------------------
// Purpose: Told when a game event message arrives (empty in CNetworkGameClientBase).
//-----------------------------------------------------------------------------
abstract_class IGameEventWatcher
{
public:
	virtual void OnGameEventReceived( INetworkMessageInternal *pEvent, const CNetMessage *pData ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Engine-facing client interface (the first 71 slots of the CNetworkGameClient
// table, the same on Windows and Linux). Names follow the Dota 2 2018 Mach-O symbols where
// the CS2 slot does the same thing; the others are named by what the code does. Not pure:
// CNetworkGameClientBase implements all of it.
//-----------------------------------------------------------------------------
class INetworkGameClient
{
public:
	// Clears the client, then keeps the socket
	virtual void Init( int nSocket );
	virtual void SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr );
	virtual int AddRef();
	virtual int Release();
	virtual CGlobalVarsBase *GetGlobals();
	virtual int GetClientTickCount() const;
	virtual int GetServerTickCount() const;
	// Named by what it does: the tick of the last packet entities update CNetworkGameClient applied
	virtual int GetLastEntityUpdateTick() const;
	// Named by what it does: the same tick GetLastServerTick returns
	virtual int GetLastReceivedServerTick() const;
	// Named by what it does: the newest command number of CNetworkGameClient's user command ring
	virtual int GetLastOutgoingCommand() const;
	// Named by what it does: whether that ring still holds command nCommand
	virtual bool HasOutgoingCommand( int nCommand ) const;
	virtual void SetFrameTime( float flFrameTime );
	virtual void Connect( CSplitScreenSlot nSlot, int nSocket, const ns_address *pAddr, int nChannelFlags, bool bSlotFlag );
	virtual void DisconnectGame( ENetworkDisconnectionReason reason );
	virtual bool IsActive() const;
	virtual bool IsConnected() const;
	virtual bool IsPaused() const;
	// Named by what it does: the local listen server has pause group 3 set
	virtual bool IsLocalServerPaused() const;
	virtual bool IsInGame() const;
	virtual bool IsBackgroundMap() const;
	virtual int GetMaxClients() const;
	virtual void Simulate( const EventClientSimulate_t &event );
	virtual void ClientPollNetworking( const EventClientPollNetworking_t &event );
	virtual void ClientProcessNetworking( const EventClientProcessNetworking_t &event );
	virtual void OnClientSendInput( const EventClientSendInput_t &event );
	virtual void OnClientFrameSimulate( const EventClientFrameSimulate_t &event );
	virtual void OnClientAdvanceTick( const EventClientAdvanceTick_t &event );
	virtual void OnClientPostAdvanceTick( const EventClientPostAdvanceTick_t &event );
	virtual void OnClientOutput( const EventClientOutput_t &event );
	virtual void OnClientPostOutput( const EventClientPostOutput_t &event );
	// Named by what it does: a per-slot network time statistic in milliseconds, -9.9 without one
	virtual float GetNetworkTimeStat( CSplitScreenSlot nSlot, bool bOutgoing ) const;
	virtual void RequestServerStatus( CSplitScreenSlot nSlot );
	virtual void ForceFullUpdate( const char *pszReason );
	virtual void PrintSpawnGroupStatus() const;
	virtual bool GetCameraPosition( CSplitScreenSlot nSlot, CTransform *pTransform );
	virtual CEntityIndex GetLocalPlayer( CSplitScreenSlot nSlot ) const;
	virtual void ServerCmd( CSplitScreenSlot nSlot, const char *pszCmd );
	virtual void ServerCmd( CSplitScreenSlot nSlot, const CCommand &args );
	virtual void SendStringCmd( CSplitScreenSlot nSlot, const char *pszCmd );
	virtual void SplitScreenConnect( CSplitScreenSlot nSlot );
	virtual int GetMaxSplitscreenPlayers() const;
	virtual INetChannel *GetNetChannel( CSplitScreenSlot nSlot );
	virtual INetChannelInfo *GetNetChannelInfo( CSplitScreenSlot nSlot );
	// Named by its place: empty in every class of this build
	virtual void OnBeforeUpdateAudioState();
	virtual void UpdateAudioState( AudioState_t *pAudioState, CSplitScreenSlot nSlot );
	virtual float GetTime() const;
	virtual void PrepareSteamConnectResponse( uint64 nSteamID, bool bLowViolence, const ns_address &addr, C2S_CONNECT_Message &msg );
	virtual bool IsLowViolence() const;
	virtual int GetHostVersion() const;
	virtual float GetLastTimeStamp() const;
	virtual int GetLastServerTick() const;
	virtual void SetGameSessionManifests( HGameResourceManifest hServerManifest, HGameResourceManifest hClientManifest );
	virtual void SpewSerializers( const char *pszFilter );
	virtual void EntityClassCompletion( const char *pszPartial, CUtlVector< CUtlString > &matches );
	virtual bool IsSpawnGroupAsyncLoaded( SpawnGroupHandle_t hSpawnGroup );
	virtual ISource2WorldSession *GetWorldSession();
	virtual const char *GetSpawnGroupName( SpawnGroupHandle_t hSpawnGroup );
	virtual void StartChangeLevel();
	virtual void FinishChangeLevel( CClientChangelevelState *pState );
	virtual bool IsChangelevelPending() const;
	virtual void GetAllLoadingSpawnGroups( CUtlVector< SpawnGroupHandle_t > *pOut );
	virtual bool ProcessConnectionlessPacket( netpacket_t *packet );
	virtual void BlockDisconnectionTracking( bool bBlock );
	virtual void TransmitNetChannel( CSplitScreenSlot nSlot );
	// Named by what it does: CNetworkGameClient's desired tick packet receive margin
	virtual float GetTickPacketReceiveMargin();
	virtual float GetTickRemainder() const;
	// Named by what it does: CNetworkGameClient asks the client DLL to fill *pOut
	virtual uint64 *GetClientDllValue( uint64 *pOut );
	// Named by what it does: the connection helper reached state 3
	virtual bool IsConnectionEstablished();
	// Pure in CNetworkGameClientBase. Named by what they do: float statistics CNetworkGameClient
	// keeps, 0 on SourceTV
	virtual float GetClientStatFloatA() = 0;
	virtual float GetClientStatFloatB() = 0;
	// The flag Connect stores for the slot
	virtual bool GetSplitScreenSlotFlag( CSplitScreenSlot nSlot );
};

//-----------------------------------------------------------------------------
// Purpose: Per split screen slot connection state of the client.
//-----------------------------------------------------------------------------
struct ClientSplitScreenSlot_t
{
	INetChannel *m_pNetChannel;
	CEntityIndex m_nLocalPlayer;
	// Named by what is known: zeroed with the slot
	int m_nSlotState;
	uint16 m_nSlotFlags;
	// The last argument of Connect
	bool m_bConnectFlag;
};

//-----------------------------------------------------------------------------
// Purpose: Server class names received in svc_ClassInfo.
//-----------------------------------------------------------------------------
struct ClientServerClassInfo_t
{
	CUtlString m_szClassName;
	// Freed with the entry when it owns it
	void *m_pClassData;
};

//-----------------------------------------------------------------------------
// Purpose: One entity baseline (CopyEntityBaseline keeps two sets of 16384).
//-----------------------------------------------------------------------------
struct ClientEntityBaseline_t
{
	// Named by what is known: cleared whenever the baseline changes
	void *m_pBaselineData;
	// The class the baseline was decoded for, CopyNewEntity reuses it only for the same class
	const void *m_pClass;
	void *m_pBaselineExtra;
	CEntityIndex m_nEntityIndex;
	int m_nBaselineFlags;
	// Serialized entity allocated by the serialized entity manager
	void *m_pSerializedEntity;
	int m_nSerialNumber;
	int m_nDataBits;
	CUtlVector< byte > m_Data;
	uint64 m_nUpdateTick;
	uint16 m_nUpdateFlags;
	bool m_bValid;
};

struct ClientEntityBaselineSet_t
{
	int m_nCount;
	ClientEntityBaseline_t m_Baselines[ 16384 ];
};

//-----------------------------------------------------------------------------
// Purpose: Concrete client base: message handlers, spawn groups, entity baselines, sign-on.
// Slots 71 and up of the CNetworkGameClient table; on Linux the destructor comes first and
// the overrides of FilterMessage, OnShutdownChannel and OnGameEventReceived follow slot 84.
//-----------------------------------------------------------------------------
class CNetworkGameClientBase : public INetworkGameClient, public CUtlSlot, protected INetworkMessageProcessingPreFilter, public IConnectionlessPacketHandler, public INetworkChannelNotify, public IGameEventWatcher
{
public:
	virtual ~CNetworkGameClientBase();

	virtual void SetTickRemainder( float flRemainder );
	// Copied into CGlobalVarsBase::m_flUsercmdTickFraction by OnClientFrameSimulate
	virtual void SetUsercmdTickFraction( float flFraction );
	virtual int ApplyClockDrift( int nServerTick, int nClientTick );
	// Named by what it does: the client DLL reports a valid slot
	virtual bool IsClientDllSlotValid();
	virtual void SendMove();
	// Named by what it does: advances the client time by the frame time
	virtual void AdvanceClientTime();
	// Named by what it does: recomputes the frame time from the tick interval and clock drift
	virtual void UpdateFrameTime();
	// Named by what it does: CNetworkGameClient runs its queued tick packets under its lock
	virtual void ProcessQueuedTickPackets();
	// Named by what it does: how late the asynchronous tick packet send runs, smoothed
	virtual float GetTickSendLateness();
	virtual void SendTickSync();
	// Named by what it does: CNetworkGameClient updates its demo/HLTV playback state
	virtual void UpdatePlaybackState();
	virtual void ClientPredictionPostNetupdate();
	// Named by what it does: CNetworkGameClient refreshes its network quality statistics
	virtual void UpdateNetworkQualityStats();
	// is_hltv of the last svc_ServerInfo (CNetworkGameClient::m_bServerIsHLTV)
	virtual bool IsServerHLTV();

	// Drops svc_PacketReliable
	virtual bool FilterMessage( const CNetMessage *pData, INetChannel *pChannel );
	virtual void OnShutdownChannel( INetChannel *pChannel, bool bTimedOut );
	virtual void OnGameEventReceived( INetworkMessageInternal *pEvent, const CNetMessage *pData );

	virtual bool GetPlayerInfo( CPlayerSlot nSlot, google::protobuf::Message &info );
	virtual CPlayerUserId GetPlayerUserId( CPlayerSlot nSlot );
	virtual void ConnectionStart( INetChannel *pChannel );
	virtual bool ProcessTick( const CNETMsg_Tick_t &msg );
	virtual bool ProcessStringCmd( const CNETMsg_StringCmd_t &msg );
	virtual bool ProcessSetConVar( const CNETMsg_SetConVar_t &msg );
	virtual bool ProcessDebugOverlay( const CNETMsg_DebugOverlay_t &msg );
	virtual bool ProcessSignonState( const CNETMsg_SignonState_t &msg );
	virtual bool ProcessSpawnGroup_Load( const CNETMsg_SpawnGroup_Load_t &msg );
	virtual bool ProcessSpawnGroup_ManifestUpdate( const CNETMsg_SpawnGroup_ManifestUpdate_t &msg );
	virtual bool ProcessSpawnGroup_Unload( const CNETMsg_SpawnGroup_Unload_t &msg );
	virtual bool ProcessSpawnGroup_SetCreationTick( const CNETMsg_SpawnGroup_SetCreationTick_t &msg );
	virtual bool ProcessPeerList( const CSVCMsg_PeerList_t &msg );
	virtual bool ProcessVoiceInit( const CSVCMsg_VoiceInit_t &msg );
	virtual bool ProcessVoiceData( const CSVCMsg_VoiceData_t &msg );
	virtual bool ProcessPrint( const CSVCMsg_Print_t &msg );
	virtual bool ProcessServerInfo( const CSVCMsg_ServerInfo_t &msg );
	virtual bool ProcessClassInfo( const CSVCMsg_ClassInfo_t &msg );
	virtual bool ProcessFlattenedSerializer( const CSVCMsg_FlattenedSerializer_t &msg );
	virtual bool ProcessMenu( const CSVCMsg_Menu_t &msg );
	virtual bool ProcessGetCvarValue( const CSVCMsg_GetCvarValue_t &msg );
	virtual bool ProcessSetPause( const CSVCMsg_SetPause_t &msg );
	virtual bool ProcessClearAllStringTables( const CSVCMsg_ClearAllStringTables_t &msg );
	virtual bool ProcessCreateStringTable( const CSVCMsg_CreateStringTable_t &msg );
	virtual bool ProcessUpdateStringTable( const CSVCMsg_UpdateStringTable_t &msg );
	virtual bool ProcessSetView( const CSVCMsg_SetView_t &msg );
	virtual bool ProcessPacketEntities( const CSVCMsg_PacketEntities_t &msg );
	virtual bool ProcessSplitScreen( const CSVCMsg_SplitScreen_t &msg );
	virtual bool ProcessCmdKeyValues( const CSVCMsg_CmdKeyValues_t &msg );
	virtual bool ProcessEncryptedData( const CSVCMsg_EncryptedData_t &msg );
	virtual bool ProcessRconServerDetails( const CSVCMsg_RconServerDetails_t &msg );
	virtual bool ProcessSounds( const CSVCMsg_Sounds_t &msg );
	virtual bool ProcessPrefetch( const CSVCMsg_Prefetch_t &msg );
	virtual bool ProcessStopSound( const CSVCMsg_StopSound_t &msg );
	virtual bool ProcessPacketReliable( const CSVCMsg_PacketReliable_t &msg );
	virtual bool ProcessFullFrameSplit( const CSVCMsg_FullFrameSplit_t &msg );
	virtual bool ProcessHLTVStatus( const CSVCMsg_HLTVStatus_t &msg );
	virtual bool ProcessServerSteamID( const CSVCMsg_ServerSteamID_t &msg );
	virtual bool ProcessConnectionClosed( const NetMessageConnectionClosed_t &msg );
	virtual bool ProcessConnectionCrashed( const NetMessageConnectionCrashed_t &msg );
	virtual bool ProcessChangeSplitscreenUser( const NetMessageSplitscreenUserChanged_t &msg );
	virtual bool ProcessPacketStart( const NetMessagePacketStart_t &msg );
	virtual bool ProcessPacketEnd( const NetMessagePacketEnd_t &msg );
	virtual bool ProcessBroadcastCommand( const CSVCMsg_Broadcast_Command_t &msg );
	virtual bool ProcessUserCmds( const CSVCMsg_UserCommands_t &msg );
	// Named by what it does: no handler in the base, CNetworkGameClient decodes a user message
	virtual bool ProcessUserMessage( const CSVCMsg_UserMessage_t &msg, int nUnused );
	virtual void Clear( bool bFull );
	virtual void FullConnect( const ns_address &addr );
	virtual void SetSignonState( SignonState_t state, int nSpawnCount, const CNETMsg_SignonState_t *pMsg );
	virtual void RunFrame();
	virtual void InstallStringTableCallback( bool bInstall );
	virtual void HookClientStringTable( bool bHook );
	virtual const char *GetClientName( CSplitScreenSlot nSlot );
	virtual void ProcessReadPacketEntitiesWorkItems( int eStatType, CEntityReadInfo &info );
	virtual void OnPreserveEntity( CEntityReadInfo &info );
	virtual void CopyNewEntity( IEntityReport *pReport, CEntityReadInfo &info, ClientClass *pClass, int nSerialNum, unsigned int nFlags ) = 0;
	virtual void CopyExistingEntity( IEntityReport *pReport, CEntityReadInfo &info ) = 0;
	virtual void OnReceivedUncompressedPacket();
	virtual void ClearState( bool bFull );

	virtual bool ProcessConnectionlessPacket( netpacket_t *packet );

public:
	ResourceGroupVector_t m_ResourceGroups[ 2 ];
	CGlobalVarsBase m_GlobalVars;
	ClientSplitScreenSlot_t m_SplitScreenSlots[ 4 ];
	// Kept across a changelevel in CClientChangelevelState
	CUtlVector< ns_address > m_ServerAddresses;
	// Connection helper (server address, challenge, state; 200 bytes)
	void *m_pConnectState;
	CUtlVector< ClientServerClassInfo_t > m_ServerClasses;
	// Named by what it does: entries released in the destructor, walked when classes are linked
	CUtlVector< void * > m_LinkedClasses;
	// Named by what is known: constructed and freed, nothing else in this build
	CUtlHashtable< uint32, void * > m_UnreferencedHashtable;
	IFlattenedSerializers *m_pFlattenedSerializers;
	CNetworkStringTableContainer *m_pStringTables;
	CUtlBuffer m_FlattenedSerializerBuffer;
	// "maps/<name>.vpk" and <name> from svc_ClearAllStringTables
	CUtlString m_szMapPackFile;
	CUtlString m_szMapName;
	uint64 m_nServerSteamID;
	int m_nMaxSpawnGroupCreationSequence;
	// The split screen slot the message being processed belongs to
	int m_nCurrentSplitScreenSlot;
	SignonState_t m_nSignonState;
	// Read from the flattened serializers in ProcessFlattenedSerializer
	int m_nSerializersCRC;
	int m_nRefCount;
	int m_Socket;
	int m_nMaxClients;
	int m_nServerCount;
	int m_nServerProtocol;
	int m_nServerTick;
	float m_flConnectTime;
	// Named by what is known: initialised (0, 0, -1) by the constructor
	int m_nUnreferencedInt0;
	int m_nUnreferencedInt1;
	float m_flUnreferencedTime;
	// Named by what is known: not initialised by the constructor and not read in this build
	byte m_UnreferencedBytes[ 32 ];
	uint64 m_nUnreferencedQword;
	int m_nUnreferencedInt2;
	// Tested by the network system's command queue send mode
	bool m_bUseClientCommandQueue;
	int m_nServerClassBits;
	// Set on every sign-on state change of CNetworkGameClient
	int m_nLastSignonStateChange;
	// Reset by Connect, compared by CNetworkGameClient when the connection closes
	int m_nConnectSequence;
	bool m_bIsHLTV;
	bool m_bPaused;
	// Named by what is known: zeroed with m_bPaused, nothing reads it in this build
	bool m_bUnreferencedFlag;
	GameSessionConfiguration_t m_GameSessionConfig;
	CThreadFastMutex m_InstanceBaselinesMutex;
	CUtlHashMap< uint32, CBaselineEntityData * > m_InstanceBaselines;
	int m_nClientTick;
	int m_nLastServerTick;
	ISource2WorldSession *m_pWorldSession;
	IGameSpawnGroupMgr *m_pSpawnGroupMgr;
	CUtlMap< SpawnGroupHandle_t, void * > m_SpawnGroups;
	// "unnamed" until set, kept across a changelevel
	CUtlString m_szConnectionName;
	CUtlLinkedList< SpawnGroupHandle_t > m_LoadingSpawnGroups;
	// Copy of m_szMapName made by Clear
	CUtlString m_szLastMapName;
	ClientEntityBaselineSet_t m_EntityBaselines[ 2 ];
	// One bit per baseline set when it changes, under m_EntityBaselinesMutex
	CBitVec< 16384 > m_EntityBaselinesChanged[ 2 ];
	// Named by what is known: initialised to -1, nothing reads it in this build
	int m_nUnreferencedIndex;
	CThreadFastMutex m_EntityBaselinesMutex;
	HGameResourceManifest m_hServerGameSessionManifest;
	HGameResourceManifest m_hClientGameSessionManifest;
	// Named by what it does: SetGameSpawnGroupMgr resets it to 0x80000000
	int m_nSpawnGroupMgrHandleBase;
	// Named by what is known: Clear zeroes it
	int m_nClearedCounter;
	int m_nMaxSplitscreenPlayers;
	// ProcessPacketReliable acts only on newer states
	int m_nReliableStateSequence;
	// Read by OnClientAdvanceTick
	int m_nAdvanceTickLimit;
	// The game directory is one of a fixed list
	bool m_bKnownGameDir;
	bool m_bReceivedServerInfo;
	bool m_bChangeLevelPending;
	bool m_bIsBackgroundMap;
	// CNetworkGameClient::SetSignonState tests it on SIGNONSTATE_PRESPAWN
	bool m_bSignonPrespawnFlag;
	bool m_bHasFlattenedSerializers;
	// Tested with packet entities flag 0x800
	bool m_bFullFrameSplitPending;
	int m_nBroadcastCommand;
	// Reset by Connect
	bool m_bConnectReset;
	bool m_bBlockDisconnectionTracking;
};

//-----------------------------------------------------------------------------
// Purpose: The client of a SourceTV relay, embedded in CHLTVServer.
//-----------------------------------------------------------------------------
class CHLTVClientState : public CNetworkGameClientBase
{
public:
	virtual float GetClientStatFloatA();
	virtual float GetClientStatFloatB();
	virtual void CopyNewEntity( IEntityReport *pReport, CEntityReadInfo &info, ClientClass *pClass, int nSerialNum, unsigned int nFlags );
	virtual void CopyExistingEntity( IEntityReport *pReport, CEntityReadInfo &info );

public:
	// The frame being read, handed to the server and freed after the packet
	CClientFrame *m_pPendingFrame;
	// What CHLTVServerAsync returned for the last frame
	void *m_pLastQueuedFrame;
	CHLTVServer *m_pHLTV;
	CFrameSnapshotManager *m_pFrameSnapshotManager;
	// hltv_update_stats timer
	float m_flNextStatsUpdateTime;
	// Set once on "SourceTV connection timed out."
	bool m_bTimedOut;
};

//-----------------------------------------------------------------------------
// Purpose: One entry of CNetworkGameClient's ring of 256 outgoing user commands.
//-----------------------------------------------------------------------------
struct ClientOutgoingCommand_t
{
	int m_nCommandNumber;
	// Named by what is known: the remaining members are cleared when the entry is reused
	int m_nCommandFlags;
	double m_flSendTime;
	uint64 m_nSendData0;
	uint64 m_nSendData1;
	// Copied from CNetworkGameClient::m_nSendTickBase when the command is sent
	int m_nSendTick;
	int m_nAckState;
	double m_flAckTime;
	// HasOutgoingCommand requires it; set by the constructor
	bool m_bValid;
};

//-----------------------------------------------------------------------------
// Purpose: One entry of CNetworkGameClient's ring of 256 received tick packets.
//-----------------------------------------------------------------------------
struct ClientTickPacket_t
{
	int m_nTick;
	int m_nCommandAck;
	float m_flHostFrameTime;
	float m_flHostComputationTime;
	// Bits 6-7 survive when the entry is consumed
	uint8 m_nFlags;
	uint64 m_nPacketData;
	uint64 m_nPacketSequence;
	// TrueViewLookAhead and the quality code treat <= 0 as not received
	double m_flReceiveTime;
	uint64 m_nConsumeData;
};

//-----------------------------------------------------------------------------
// Purpose: Network quality samples CNetworkGameClient keeps per direction
// (UpdateNetworkQualityMessages reads them).
//-----------------------------------------------------------------------------
struct ClientNetQualityStats_t
{
	// Incremented per tick by bucket
	int m_nTickBuckets[ 7 ];
	// Summed into the quality message
	int m_nQualityCounters[ 7 ];
	// GetClientStatFloatA / B return it
	float m_flAverage;
	int m_nTotal;
	int m_nDropped;
	// Number of valid entries of m_SmallSamples
	int m_nSmallSampleCount;
	int m_nSmallSampleSum;
	// m_SmallSamples must be sorted before a percentile is read
	bool m_bSmallSamplesUnsorted;
	uint8 m_SmallSamples[ 103 ];
	// Reservoir of up to 500 samples (random replacement once full)
	int m_nSampleCount;
	int m_nSamplesSeen;
	bool m_bSamplesDirty;
	int16 m_Samples[ 500 ];
	int16 m_nSampleMax;
	int16 m_nSampleMin;
};

class CReadPacketEntities_ParallelProcessingController;

//-----------------------------------------------------------------------------
// Purpose: The game's client. Adds no virtual method to the CNetworkGameClientBase table on
// Windows; on Linux its ISwitchLoopModeStatusNotify override is appended as slot 149.
//-----------------------------------------------------------------------------
class CNetworkGameClient : public CNetworkGameClientBase, public ISwitchLoopModeStatusNotify
{
public:
	virtual float GetClientStatFloatA();
	virtual float GetClientStatFloatB();
	virtual void CopyNewEntity( IEntityReport *pReport, CEntityReadInfo &info, ClientClass *pClass, int nSerialNum, unsigned int nFlags );
	virtual void CopyExistingEntity( IEntityReport *pReport, CEntityReadInfo &info );

	virtual void OnSwitchLoopModeFinished( const char *pszLoopModeName, uint32 nId, bool bSuccess );

public:
	CClientFrameManager m_FrameManager;
	// Entries of the "genericprecache" string table (SetGeneric)
	int m_nGenericPrecacheCount;
	CPrecacheItem m_GenericPrecache[ 512 ];
	CUtlVector< CMsgPlayerInfo > m_PlayerInfos;
	INetworkStringTable *m_pGenericPrecacheTable;
	INetworkStringTable *m_pInstanceBaselineTable;
	INetworkStringTable *m_pUserInfoTable;
	INetworkStringTable *m_pServerQueryInfoTable;
	CReadPacketEntities_ParallelProcessingController *m_pReadPacketEntitiesController;
	// is_hltv of the last svc_ServerInfo
	bool m_bServerIsHLTV;
	// Cleared by packet entities processing
	bool m_bEntityUpdatePending;
	// Set by the constructor and Clear, tested at the start of ClientProcessNetworking
	bool m_bProcessNetworkingFirstTick;
	float m_flTickRemainder;
	float m_flUsercmdTickFraction;
	float m_flFrameTime;
	// -1 on every sign-on state change, compared with the demo playback tick
	int m_nSignonDemoTick;
	// GetLastServerTick copied at the start of ClientPollNetworking
	int m_nPolledServerTick;
	int m_nLastEntityUpdateTick;
	int m_nCommandBase;
	int m_nLastOutgoingCommand;
	int m_nLastAcknowledgedCommand;
	int m_nTickPacketMarginTicks;
	// Non-zero blocks ProcessTick and ApplyClockDrift (demo playback)
	int m_nPlaybackBlock;
	// Host timing of the last net_Tick
	int m_nHostFrameTimeUs;
	int m_nHostFrameTimeStdDevUs;
	int m_nHostLoss;
	int m_nHostComputationTimeUs;
	float m_flHostTimeScale;
	int m_nLastTickMsgTick;
	CUtlString m_szLastTickMsgOrigin;
	ClientOutgoingCommand_t m_OutgoingCommands[ 256 ];
	int m_nStatsTickBase;
	int m_nStatsTickNext;
	ClientNetQualityStats_t m_NetQualityStats[ 2 ];
	float m_flTickPacketReceiveMargin;
	ClientTickPacket_t m_TickPackets[ 256 ];
	int m_nQueuedTickPackets;
	int m_nTickPacketsBehind;
	bool m_bTrueViewLookAheadReset;
	float m_flTrueViewLookAhead;
	float m_flTrueViewLookAheadTarget;
	float m_flTrueViewLookAheadSmoothed;
	int m_nTrueViewLookAheadTicks;
	int m_nTrueViewLookAheadExcess;
	double m_flTrueViewLookAheadChangeTime;
	// GetNetworkTimeStat: [ slot ][ outgoing ]
	float m_flNetworkTimeStats[ 8 ][ 2 ];
	byte m_SendBufferMemory[ 8192 ];
	bf_write m_SendBuffer;
	// "SentTickSync cmds %d...%d"
	int m_nSendCommandFirst;
	int m_nSendCommandLast;
	int m_nAsyncSendTick;
	double m_flAsyncSendTime;
	CThreadFastMutex m_SendMutex;
	// Released when demo playback jumps
	CUtlVector< void * > m_DemoJumpResources;
	int m_nSendSequence;
	double m_flLastSendTime;
	int m_nSendTickBase;
	float m_flTickSendLateness;
	int m_nQualityMessageTickIn;
	int m_nQualityMessageTickOut;
	CMsgSource2NetworkFlowQuality m_FlowQuality[ 2 ];
	// Cleared when demo playback jumps
	void *m_pDemoJumpState0;
	void *m_pDemoJumpState1;
};

#endif // ICLIENT_H
