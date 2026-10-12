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
#include <engine2/inetworkclientservice.h>
#include <playerslot.h>
#include <ns_address.h>
#include <spawngrouptypes.h>
#include <resourcefile/resourcetype.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

class INetChannel;
class CNetworkGameClient;
class CClientChangelevelState;
class CGlobalVars;
class IGameSpawnGroupMgr;
class CCommand;
class CTransform;
class ClientClass;
class IEntityReport;
class CEntityReadInfo;
class bf_read;
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
class CSVCMsg_Broadcast_Command_t;
class CSVCMsg_UserCommands_t;
class NetMessageConnectionClosed_t;
class NetMessageConnectionCrashed_t;
class NetMessageSplitscreenUserChanged_t;
class NetMessagePacketStart_t;
class NetMessagePacketEnd_t;

//-----------------------------------------------------------------------------
// Purpose: Engine-facing client interface. Implemented by CNetworkGameClientBase.
// Vtable indices are absolute (primary vtable of CNetworkGameClient, engine2.dll).
//-----------------------------------------------------------------------------
abstract_class INetworkGameClient
{
public:
	virtual void Init( int nUnk ) = 0;
	virtual void SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;
	virtual int AddRef() = 0;
	virtual int Release() = 0;

	// Returns &m_GlobalVars (embedded CGlobalVars). Client-side globals getter.
	virtual CGlobalVars *GetGlobals() = 0;

	virtual int GetClientTickCount() const = 0;
	virtual int GetServerTickCount() const = 0;
	virtual int unk007() = 0;
	virtual int unk008() = 0;
	virtual int unk009() = 0;
	virtual bool unk010( int nUnk ) = 0;
	virtual void SetFrameTime( float flFrameTime ) = 0;
	virtual void Connect( CSplitScreenSlot nSlot, int nUnk, const ns_address &addr ) = 0;
	virtual void Disconnect( ENetworkDisconnectionReason reason ) = 0;
	virtual bool IsActive() const = 0;
	virtual bool IsConnected() const = 0;
	virtual bool IsPaused() const = 0;
	virtual bool unk017() = 0;
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
	virtual void *unk028() = 0;
	virtual void *unk029() = 0;
	virtual void *unk030() = 0;
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
	virtual void PrepareSteamConnectResponse( uint64 nUnk, bool bUnk, const ns_address &addr, void *pMsg ) = 0;
	virtual bool IsLowViolence() const = 0;
	virtual int GetHostVersion() const = 0;
	virtual float GetLastTimeStamp() const = 0;
	virtual int GetLastServerTick() const = 0;
	virtual void SetGameSessionManifests( HGameResourceManifest hManifest1, HGameResourceManifest hManifest2 ) = 0;
	virtual void SpewSerializers( const char *pszName ) = 0;
	virtual void EntityClassCompletion( const char *pszPartial, CUtlVector< CUtlString > &commands ) = 0;
	virtual bool IsSpawnGroupAsyncLoaded( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual void *GetWorldSession() = 0;
	virtual const char *GetSpawnGroupName( SpawnGroupHandle_t hSpawnGroup ) = 0;
	virtual void StartChangeLevel() = 0;
	virtual void FinishChangeLevel( CClientChangelevelState *pState ) = 0;
	virtual bool IsChangelevelPending() const = 0;
	virtual void GetAllLoadingSpawnGroups( CUtlVector< SpawnGroupHandle_t > *pSpawnGroups ) = 0;
	virtual bool ProcessConnectionlessPacket( const ns_address &addr, bf_read &buf ) = 0;
	virtual void BlockDisconnectionTracking( bool bBlock ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Concrete client base. Adds the network message Process* handlers,
// clock-drift / prediction internals and the sign-on state machine.
// Overrides of the secondary bases (message pre-filter, connectionless packet
// handler, channel notify, game event watcher) live in their own vtables on
// Windows and are not declared here.
//-----------------------------------------------------------------------------
class CNetworkGameClientBase : public INetworkGameClient
{
public:
	virtual void TransmitNetChannel() = 0;
	virtual void *unk064() = 0;
	virtual void *unk065() = 0;
	virtual void *unk066() = 0;
	virtual void *unk067() = 0;
	virtual void *unk068() = 0;
	virtual void *unk069() = 0;
	virtual void *unk070() = 0;
	virtual void *unk071() = 0;
	virtual void *unk072() = 0;
	virtual void *unk073() = 0;
	virtual void ApplyClockDrift( int nServerTick, int nClientTick ) = 0;
	virtual void *unk075() = 0;
	virtual void *unk076() = 0;
	virtual void *unk077() = 0;
	virtual void *unk078() = 0;
	virtual void *unk079() = 0;
	virtual void *unk080() = 0;
	virtual void *unk081() = 0;
	virtual void *unk082() = 0;
	virtual void *unk083() = 0;
	virtual void *unk084() = 0;
	virtual void *unk085() = 0;
	virtual void *unk086() = 0;
	virtual void *unk087() = 0;

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
	virtual bool ProcessBroadcast_Command( const CSVCMsg_Broadcast_Command_t &msg ) = 0;
	virtual bool ProcessUserCommands( const CSVCMsg_UserCommands_t &msg ) = 0;

public:
	virtual void *unk131() = 0;
	virtual void Clear( bool bUnk ) = 0;
	virtual void FullConnect( const ns_address &addr ) = 0;
	virtual void SetSignonState( SignonState_t state, int nSpawnCount, const CNETMsg_SignonState_t *pMsg ) = 0;
	virtual void *unk135() = 0;
	virtual void *unk136() = 0;
	virtual void *unk137() = 0;
	virtual void *unk138() = 0;
	virtual void *unk139() = 0;
	virtual void OnPreserveEntity( CEntityReadInfo &readInfo ) = 0;
	virtual void CopyNewEntity( IEntityReport *pReport, CEntityReadInfo &readInfo, ClientClass *pClientClass, int nUnk, unsigned int nUnk2 ) = 0;
	virtual void CopyExistingEntity( IEntityReport *pReport, CEntityReadInfo &readInfo ) = 0;
	virtual void OnReceivedUncompressedPacket() = 0;
	virtual void *unk144() = 0;
};

// The most-derived client class. Overrides a handful of base virtuals but
// adds no new vtable slots, so it is layout-compatible with the base.
class CNetworkGameClient : public CNetworkGameClientBase
{
private:
	char pad_0[ 200 ];
public:
	bool m_bInSimulation;

private:
	char pad_1[ 47 ];

public:
	bool m_bAllowClientSidePredict;

private:
	char pad_2[ 331 ];

public:
	int32 m_nDeltaTick;

private:
	char pad_3[ 296 ];

public:
	int32 m_nClientTick;
	int32 m_nServerTick;

};

#endif // ICLIENT_H
