//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef IHLTVDIRECTOR_H
#define IHLTVDIRECTOR_H
#ifdef _WIN32
#pragma once
#endif

class IHLTVServer;
class KeyValues;
class Vector;
class CSteamID;
class INetworkMessageInternal;
class CNetMessage;
class bf_write;
struct NetMessageInfo_t;

#define INTERFACEVERSION_HLTVDIRECTOR			"Source2GameDirector001"

class IHLTVDirector
{
public:
	virtual	~IHLTVDirector() {}

	virtual bool	IsActive( void ) = 0; // true if director is active

	virtual void AddHLTVServer( int nType, IHLTVServer *hltv ) = 0;
	virtual void RemoveHLTVServer( int nType, IHLTVServer *hltv ) = 0;

	virtual int GetHLTVServerCount( int nType ) = 0;
	virtual IHLTVServer* GetHLTVServer( int nType, int instance ) = 0; // get HLTV server interface of instance

	virtual int		GetDirectorTick( void ) = 0;	// get current broadcast tick from director
	virtual int		GetPVSEntity( void ) = 0;
	virtual Vector	GetPVSOrigin( void ) = 0; // get current PVS origin
	virtual float	GetDelay( void ) = 0; // returns current delay in seconds

	virtual const char**	GetModEvents() = 0;

	// Broadcasts the hltv_versioninfo event
	virtual void	OnStartRecording() = 0;
	// Empty in the server
	virtual void	OnHLTVClientDisconnect( void *p ) = 0;
	// Returns -1 in the base director
	virtual int		GetDirectorCameraEventId() = 0;
	virtual void	SendTitle( int nPlayerSlot ) = 0;
	// Builds the hltv_chat event message that the TV clients relay to the other TV clients
	virtual void	CreateLocalChatMessage( const CSteamID &steamIDTalker, const char *pszName, const char *pszChat, INetworkMessageInternal *&pOutMessage, CNetMessage *&pOutData ) = 0;
	virtual void	OnHLTVUncompressedSnapshot( int nPlayerSlot, bf_write &buf ) = 0;
	virtual void	OnHLTVNetMessage( const NetMessageInfo_t *pInfo, const CNetMessage *pData ) = 0;
	// Per-frame director update driven by the HLTV server.
	virtual void	unk018( float flUnk ) = 0;
};

#endif // IHLTVDIRECTOR_H
