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

#include <IEngineService.h>
#include <splitscreenslot.h>
#include <playerslot.h>
#include <inetchannel.h>
#include <networkbasetypes.pb.h>

class CNetworkGameClient;
class IGameSpawnGroupMgr;
class INetworkMessageInternal;
class CNetMessage;
class CCommand;
class KeyValues;
class ILoopModePrerequisiteRegistry;

abstract_class INetworkClientService : public IEngineService
{
public:
	virtual CNetworkGameClient *GetNetworkGameClient() = 0;
	virtual void SetGameSpawnGroupMgr( IGameSpawnGroupMgr *pMgr ) = 0;

	virtual void AddClientPrerequisites( int nUnk, const char *pszName, const char *pszUrl, bool bUnk, ILoopModePrerequisiteRegistry *pRegistry, KeyValues *pOptions ) = 0;

	virtual void StartupClient( KeyValues *pLoopOptions ) = 0;
	virtual void DisconnectGameNow( ENetworkDisconnectionReason reason ) = 0;
	virtual void unk028( ENetworkDisconnectionReason reason, const char *pszReason ) = 0;

	virtual void PrintSpawnGroupStatus() = 0;

	// Forwards the local player of nSlot to the game client
	virtual bool unk030( CSplitScreenSlot nSlot, void *pUnk ) = 0;

	virtual bool IsInGame() = 0;
	virtual bool IsConnected() = 0;
	virtual bool IsMultiplayer() = 0;
	virtual bool unk034() = 0;
	virtual bool unk035() = 0;
	// Returns "Server Shutting Down"
	virtual const char *unk036() = 0;

	// -1 while not connected
	virtual CPlayerSlot GetLocalPlayerSlot( CSplitScreenSlot nSlot ) = 0;

	virtual void ServerCmd( int nCommandSrc, const CCommand &args ) = 0;
	virtual void SendStringCmd( CSplitScreenSlot nSlot, const char *pszCmd ) = 0;
	virtual void SplitScreenConnect( CSplitScreenSlot nSlot ) = 0;
	virtual int GetMaxSplitScreenPlayers() = 0;
	virtual INetChannel *GetNetChannel( CSplitScreenSlot nSlot ) = 0;

	virtual double unk043( double flTime ) = 0;
	// Empty in this build
	virtual void unk044() = 0;
	virtual float unk045() = 0;
	virtual float unk046() = 0;
	virtual float unk047() = 0;
	virtual bool unk048() = 0;
	virtual void *unk049() = 0;

	// Off the main thread the message is queued and sent later
	virtual bool SendNetMessage( CSplitScreenSlot nSlot, const CNetMessage *pData, NetChannelBufType_t bufType ) = 0;

	virtual void Unk_OnGameSessionManifestReceived( void *p ) = 0;
	virtual void *unk052() = 0;

	virtual void StartChangeLevel() = 0;
	virtual bool FinishChangeLevel() = 0;
	virtual bool IsChangeLevelPending() = 0;

	// Socket opened for the client
	virtual int GetClientSocket() = 0;
	// Returns false in this build
	virtual bool unk057() = 0;
	virtual void unk058() = 0;

	virtual void *GetNetMessageDispatcher() = 0;
	virtual void RegisterNetMessage( INetworkMessageInternal *pNetMessage, bool bUnk ) = 0;

	// Client and SourceTV status lines
	virtual void PrintStatus( int nUnk1, int nUnk2 ) = 0;

	virtual IGameSpawnGroupMgr *GetGameSpawnGroupMgr() = 0;

	virtual void unk063() = 0;
};

#endif // INETWORKCLIENTSERVICE_H
