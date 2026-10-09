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
class IGameEvent;
class CUtlBuffer;
class CSteamID;
struct netadr_t;
class CGameInfo;

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
	// AMNOTE: Sums the hltv_proxies, hltv_slots and hltv_clients of the connected relay proxies
	virtual void	GetRelayStats( int &proxies, int &slots, int &specs ) = 0;
	virtual void	GetGlobalStats( int &proxies, int &slots, int &specs ) = 0; 

	// AMNOTE: Returns this server's own public address with its SourceTV port, on master and relay alike
	virtual const netadr_t *unk101( void ) = 0;
	virtual const netadr_t *GetRelayAddress( void ) = 0; // returns relay address

	virtual bool	IsMasterProxy( void ) = 0; // true, if this is the HLTV master proxy
	virtual bool	IsTVRelay( void ) = 0;
	virtual bool	IsDemoPlayback( void ) = 0; // true if this is a HLTV demo
	virtual bool	IsActive( void ) = 0;

	virtual void	BroadcastEvent(IGameEvent *event) = 0; // send a director command to all specs

	virtual bool IsRecording(void) = 0;
	virtual const char *GetRecordingDemoFilename(void) = 0;
	// AMNOTE: Returns the tick of the first broadcast frame, -1 until one arrives
	virtual int unk201( void ) = 0;
	virtual int StartAutoRecording(void) = 0;
	virtual void StopRecording(CGameInfo const *info) = 0;
	virtual void AddSaveGame( CSteamID steamID, uint64, int, const CUtlBuffer &buf ) = 0;
};

#endif
