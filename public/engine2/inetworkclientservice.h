//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engine service that owns the network game client
//
//=============================================================================//

#ifndef INETWORKCLIENTSERVICE_H
#define INETWORKCLIENTSERVICE_H
#ifdef _WIN32
#pragma once
#endif

#include <engine/IEngineService.h>
#include <eiface.h>
#include <splitscreenslot.h>
#include <playerslot.h>
#include <networksystem/inetchannel.h>
#include <networkbasetypes.pb.h>
#include <steam/steamclientpublic.h>
#include <tier0/convar.h>
#include <tier0/tslist.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlsignalslot.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <vconcomm/ivconcomm.h>

class CNetworkGameClient;
class IGameSpawnGroupMgr;
class INetworkMessageInternal;
class CNetMessage;
class CCommand;
class KeyValues;
class ILoopModePrerequisiteRegistry;
class CTransform;
class IEntityReport;
class CCompressedResourceManifest;
class CClientChangelevelState;
class CGameClientConnectPrerequisite;
class CVConsole2FlattenedSerializerListener;

abstract_class INetworkClientService : public IEngineService
{
public:
	virtual CNetworkGameClient *GetNetworkGameClient() = 0;
	virtual void SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;

	// bLocalServer is set for the "listenserver" and "dedicated" loop modes
	virtual void AddClientPrerequisites( int nType, const char *pszName, const char *pszUrl, bool bLocalServer, ILoopModePrerequisiteRegistry *pRegistry, KeyValues *pOptions ) = 0;

	virtual void StartupClient( KeyValues *pLoopOptions ) = 0;
	virtual void DisconnectGameNow( ENetworkDisconnectionReason reason ) = 0;
	virtual void DisconnectGameQueued( ENetworkDisconnectionReason reason, const char *pszReason ) = 0;

	virtual void PrintSpawnGroupStatus() = 0;

	virtual bool GetCameraPosition( CSplitScreenSlot nSlot, CTransform *pTransform ) = 0;

	virtual bool IsInGame() = 0;
	virtual bool IsConnected() = 0;
	virtual bool IsMultiplayer() = 0;
	virtual bool IsPaused() = 0;
	virtual bool IsDisconnecting() = 0;
	// Returns "Server Shutting Down"
	virtual const char *GetDisconnectReason() = 0;

	// -1 while not connected
	virtual CPlayerSlot GetLocalPlayerSlot( CSplitScreenSlot nSlot ) = 0;

	virtual void ServerCmd( int nCommandSrc, const CCommand &args ) = 0;
	virtual void SendStringCmd( CSplitScreenSlot nSlot, const char *pszCmd ) = 0;
	virtual void SplitScreenConnect( CSplitScreenSlot nSlot ) = 0;
	virtual int GetMaxSplitScreenPlayers() = 0;
	virtual INetChannel *GetNetChannel( CSplitScreenSlot nSlot ) = 0;

	virtual double ClockDriftAdjustFrameTime( double flFrameTime ) = 0;
	// Empty in this build
	virtual void unk044() = 0;
	// INetworkGameClient::GetTime of the game client, 0 without one
	virtual float GetTime() = 0;
	virtual float GetClientTime() = 0;
	// CNetworkGameClient::m_flFrameTime, 0 while paused
	virtual float GetFrameTime() = 0;
	virtual bool IsEntityReportActive() = 0;
	virtual IEntityReport *GetEntityReport() = 0;

	// Off the main thread the message is queued and sent later
	virtual bool SendNetMessage( CSplitScreenSlot nSlot, const CNetMessage *pData, NetChannelBufType_t bufType ) = 0;

	virtual void ReceivedServerInfo( const CCompressedResourceManifest &manifest ) = 0;
	// Registry of the startup resource prerequisite of the connect prerequisite
	virtual ILoopModePrerequisiteRegistry *GetGameSessionInitPrerequisites() = 0;

	virtual void StartChangeLevel() = 0;
	virtual bool FinishChangeLevel() = 0;
	virtual bool IsChangeLevelPending() = 0;

	// EventClientAdvanceTick_t::m_nTotalTicks of the last client tick; the demo and replay players count their ticks from it
	virtual int GetTotalTicks() = 0;
	// Returns false in this build
	virtual bool SteamIDAllowedToP2PConnect( CSteamID steamID ) = 0;
	virtual void OnLevelLoadingStarted() = 0;

	virtual CEventDispatcher< CEventIDManager_Default > *GetNetMessageDispatcher() = 0;
	// Messages the client receives get a relay entry that forwards them to GetNetMessageDispatcher(); bClientToServer messages only get their channel category
	virtual void RegisterNetMessage( INetworkMessageInternal *pNetMessage, bool bClientToServer ) = 0;

	// Client and SourceTV status lines; nFlags has 2 for "status short" and 4 for "status full"
	virtual void PrintStatus( CommandTarget_t nTarget, int nFlags ) = 0;

	virtual IGameSpawnGroupMgr *GetGameSpawnGroupMgr() = 0;

	// While the net graph is active, sends each entry of m_CommandQueueInfoHistory to VConsole as KeyValues3
	// (server_cmd_queue, starved_ticks, discarded_ticks, server_tick, tick_interval, server_cmd_recv_margin) and clears it
	virtual void SendCommandQueueInfoToVConsole() = 0;
};

class CNetworkClientService : public CBaseEngineService< INetworkClientService >, public CUtlSlot, public IVConCommDataReceived
{
public:
	// Registered on the net channel for each message RegisterNetMessage relays
	struct NetMessageRelay_t : public CUtlSlot
	{
		CNetworkClientService *m_pService;
		INetworkMessageInternal *m_pNetMessage;
		// NetMessageInfo_t::m_MessageId of the message, dispatched with it
		int m_nMessageId;
	};

	// Pushed by SendNetMessage off the main thread, sent on the main thread
	struct QueuedNetMessage_t
	{
		CSplitScreenSlot m_nSlot;
		CNetMessage *m_pData; // Clone of the message
		NetChannelBufType_t m_nBufType;
	};

	uint8 m_unk068;
	bool m_bEntityReportActive;
	bool m_bDisconnecting;
	ENetworkDisconnectionReason m_nDisconnectReason;
	CUtlString m_sDisconnectReason;
	// Addons mounted when DisconnectGameQueued was called, passed on when the queued disconnect runs
	CUtlLeanVector< CUtlString > m_DisconnectAddons;
	IEntityReport *m_pEntityReport;
	int m_nTotalTicks;
	// Client socket created by CNetworkService::OpenSockets, passed to the game client
	int m_nClientSocket;
	CGameClientConnectPrerequisite *m_pConnectPrerequisite;
	CNetworkGameClient *m_pNetworkGameClient;
	CTSQueue< QueuedNetMessage_t > m_QueuedNetMessages;
	CClientChangelevelState *m_pChangelevelState;
	// The last 20 command queue infos received by the game client while the net graph is active
	CUtlVector< ClientCommandQueueInfo_t > m_CommandQueueInfoHistory;
	CUtlVector< NetMessageRelay_t * > m_NetMessageRelays;
	CEventDispatcher< CEventIDManager_Default > m_NetMessageDispatcher;
	CVConsole2FlattenedSerializerListener *m_pFlattenedSerializerListener;
	bool m_bUnk170;
	CConCommandMemberAccessor< CNetworkClientService > m_DisconnectCommand;
	CConCommandMemberAccessor< CNetworkClientService > m_StatusCommand;
	CConCommandMemberAccessor< CNetworkClientService > m_FullUpdateCommand;
	CConCommandMemberAccessor< CNetworkClientService > m_PauseCommand;
	CConCommandMemberAccessor< CNetworkClientService > m_SetPauseCommand;
	CConCommandMemberAccessor< CNetworkClientService > m_UnpauseCommand;
};

#endif // INETWORKCLIENTSERVICE_H
