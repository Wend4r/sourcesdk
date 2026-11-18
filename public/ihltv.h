//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#ifndef IHLTV_H
#define IHLTV_H

#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

class CNetworkGameServerBase;
class CPlayerSlot;
class IHLTVDirector;
class INetworkMessageInternal;
class CNetMessage;
class CUtlBuffer;
class CSteamID;
struct netadr_t;
class CGameInfo;
class CClientFrame;
class CJob;

//-----------------------------------------------------------------------------
// Interface the HLTV module exposes to the engine
//-----------------------------------------------------------------------------
#define INTERFACEVERSION_HLTVSERVER	"HLTVServer001"

class CGameInfo;

class IHLTVServer : public IBaseInterface
{
public:
	virtual	~IHLTVServer() {}

	virtual	CNetworkGameServerBase *GetBaseServer( void ) = 0; // get HLTV base server interface
	virtual	IHLTVDirector *GetDirector( void ) = 0;	// get director interface
	virtual	CPlayerSlot GetHLTVSlot( void ) = 0; // return the player slot of HLTV in game
	virtual float	GetOnlineTime( void ) = 0; // seconds since broadcast started
	virtual void	GetLocalStats( int &proxies, int &slots, int &specs ) = 0;
	// Sums the hltv_proxies, hltv_slots and hltv_clients of the connected relay proxies
	virtual void	GetRelayStats( int &proxies, int &slots, int &specs ) = 0;
	virtual void	GetGlobalStats( int &proxies, int &slots, int &specs ) = 0;

	// Named by what it does: this server's own public address with its SourceTV port, on master and relay alike
	virtual const netadr_t *GetPublicAddress( void ) = 0;
	virtual const netadr_t *GetRelayAddress( void ) = 0; // returns relay address

	virtual bool	IsMasterProxy( void ) = 0; // true, if this is the HLTV master proxy
	virtual bool	IsTVRelay( void ) = 0;
	virtual bool	IsDemoPlayback( void ) = 0; // true if this is a HLTV demo
	virtual bool	IsActive( void ) = 0;

	virtual void	BroadcastEvent( INetworkMessageInternal *pEvent, const CNetMessage *pData ) = 0; // send a director command to all specs

	virtual bool IsRecording(void) = 0;
	virtual const char *GetRecordingDemoFilename(void) = 0;
	// Named by what it does: the tick of the first broadcast frame, -1 until one arrives
	virtual int GetBroadcastStartTick( void ) = 0;
	virtual int StartAutoRecording(void) = 0;
	virtual void StopRecording(CGameInfo const *info) = 0;
	virtual void AddSaveGame( CSteamID steamID, uint64, int, const CUtlBuffer &buf ) = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Receives the frames and messages of a SourceTV stream (CHLTVServer implements it for
// its asynchronous broadcast half). No method name of this interface survives in the binaries:
// each one is named by what CHLTVServer does in it.
//-----------------------------------------------------------------------------
class ITvStreamSink
{
public:
	// Takes the next tick's frame: the first one sets the broadcast start tick and activates the
	// server, then the signon buffers of the asynchronous half are swapped
	virtual bool BeginFrame( const CClientFrame *pFrame ) = 0;
	// Copies the frame (tick, snapshot, transmit bits) for the clients and the demo recorder
	virtual void AddFrame( const CClientFrame *pFrame ) = 0;
	// The pending send-client-messages job of the asynchronous half
	virtual CJob *GetSendJob( void ) = 0;
	// The tick of the last frame given to AddFrame
	virtual int GetLastFrameTick( void ) = 0;
	// Writes a net message to the broadcast and the recording
	virtual bool SendNetMessage( const CNetMessage *pData, bool bReliable ) = 0;
};

#endif
