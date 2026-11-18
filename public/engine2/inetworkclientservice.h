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
#include <splitscreenslot.h>
#include <playerslot.h>
#include <networksystem/inetchannel.h>
#include <networkbasetypes.pb.h>

class CNetworkGameClient;
class IGameSpawnGroupMgr;
class INetworkMessageInternal;
class CNetMessage;
class CCommand;
class KeyValues;
class ILoopModePrerequisiteRegistry;
class CTransform;

// AMNOTE: Names come from the CNetworkClientService vtable of Dota 2 2018 (libengine2.dylib symbols) where the CS2
// slot does the same thing; the others are named by what the code does.
abstract_class INetworkClientService : public IEngineService
{
public:
	virtual CNetworkGameClient *GetNetworkGameClient() = 0;
	virtual void SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;

	// What the bool selects is not established
	virtual void AddClientPrerequisites( int nPrerequisitesType, const char *pszName, const char *pszUrl, bool, ILoopModePrerequisiteRegistry *pRegistry, KeyValues *pOptions ) = 0;

	virtual void StartupClient( KeyValues *pLoopOptions ) = 0;
	virtual void DisconnectGameNow( ENetworkDisconnectionReason reason ) = 0;
	// Stores the reason and marks the disconnect as pending instead of disconnecting right away
	virtual void DisconnectGame( ENetworkDisconnectionReason reason, const char *pszReason ) = 0;

	virtual void PrintSpawnGroupStatus() = 0;

	// Asks ISource2Client::GetCameraPosition for the local player of nSlot
	virtual bool GetCameraPosition( CSplitScreenSlot nSlot, CTransform *pTransform ) = 0;

	virtual bool IsInGame() = 0;
	virtual bool IsConnected() = 0;
	virtual bool IsMultiplayer() = 0;
	virtual bool IsPaused() = 0;
	// True while a disconnect is pending or there is no network game client
	virtual bool IsDisconnectedOrDisconnecting() = 0;
	// Returns "Server Shutting Down"
	virtual const char *GetDisconnectReason() = 0;

	// -1 while not connected
	virtual CPlayerSlot GetLocalPlayerSlot( CSplitScreenSlot nSlot ) = 0;

	virtual void ServerCmd( int nCommandSrc, const CCommand &args ) = 0;
	virtual void SendStringCmd( CSplitScreenSlot nSlot, const char *pszCmd ) = 0;
	virtual void SplitScreenConnect( CSplitScreenSlot nSlot ) = 0;
	virtual int GetMaxSplitScreenPlayers() = 0;
	virtual INetChannel *GetNetChannel( CSplitScreenSlot nSlot ) = 0;

	// Applies the client clock correction (cl_clock_recvmargin, TrueView look-ahead) to the frame time; returns it
	// unchanged without a network game client
	virtual double AdjustFrameTime( double flFrameTime ) = 0;
	// Empty in this build
	virtual void GetViewEntity( CSplitScreenSlot nSlot ) = 0;
	// Client tick plus the tick fraction, in seconds; without the fraction while the client is paused
	virtual float GetClientTime() = 0;
	// Named by what it does: the same with the tick fraction also while paused
	virtual float GetUnpausedClientTime() = 0;
	// Dota's took a float; this one reads a stored value of the network game client, 0.0 while paused
	virtual float ComputeNextRenderTime() = 0;
	virtual bool IsEntityReportActive() = 0;
	virtual void *GetEntityReport() = 0;

	// Off the main thread the message is queued and sent later
	virtual bool SendNetMessage( CSplitScreenSlot nSlot, const CNetMessage *pData, NetChannelBufType_t bufType ) = 0;

	// Loads the "Client GameSessionManifest received from server"
	virtual void ReceivedServerInfo( const void *pCompressedManifest ) = 0;
	virtual void *GetGameSessionInitPrerequisites() = 0;

	virtual void StartChangeLevel() = 0;
	virtual bool FinishChangeLevel() = 0;
	virtual bool IsChangeLevelPending() = 0;

	// Socket opened for the client
	virtual int GetClientSocket() = 0;
	// Returns false in this build
	virtual bool SteamIDAllowedToP2PConnect( uint64 nSteamID ) = 0;
	// Calls ISource2Client::CenterStringOff and OnLevelLoadingStarted, then two methods of the game spawn group manager
	virtual void OnLevelLoadingStarted() = 0;

	virtual void *GetNetMessageDispatcher() = 0;
	// What the bool selects is not established; it changes the mode the net message is registered with
	virtual void RegisterNetMessage( INetworkMessageInternal *pNetMessage, bool ) = 0;

	// Client and SourceTV status lines; what the bool selects is not established
	virtual void PrintStatus( int nCommandTarget, bool ) = 0;

	virtual IGameSpawnGroupMgr *GetGameSpawnGroupMgr() = 0;

	// Named by what it does: serializes a state message of the network game client and hands it on, only while
	// the network system reports the feature as enabled
	virtual void PublishClientStateMessage() = 0;
};

#endif // INETWORKCLIENTSERVICE_H
