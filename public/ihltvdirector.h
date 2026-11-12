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
class bf_write;
class CNetMessage;
class INetworkMessageInternal;
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

	// Fires the "hltv_versioninfo" event to every HLTV server; the game implementation reads no argument.
	virtual void	Unk_SendVersionInfo( void *p ) = 0;
	// Empty in the game implementation.
	virtual void	Unk_OnHLTVClientDisconnect( void *p ) = 0;
	// Id of the legacy game event that CaptureCameraEvent() keeps as the last camera event, -1 when there is none.
	virtual int		GetCameraEventId() = 0;
	// Sends the "hltv_title" event with the tv_title text to nPlayerSlot, or to every player when it is -1.
	virtual void	SendTitle( int nPlayerSlot ) = 0;
	// Builds the "hltv_chat" event and serializes it into a new legacy game event message.
	// On failure *ppMessage is set to NULL.
	virtual void	SerializeChat( const CSteamID &steamID, const char *pszName, const char *pszText, INetworkMessageInternal **ppSerializer, CNetMessage **ppMessage ) = 0;
	// Writes the last camera event, if any, into the snapshot buffer.
	virtual void	OnHLTVUncompressedSnapshot( int nPlayerSlot, bf_write &buf ) = 0;
	// Stores a copy of the message when it is a legacy game event whose id is GetCameraEventId().
	virtual void	CaptureCameraEvent( const NetMessageInfo_t *pInfo, const CNetMessage *pData ) = 0;
	// Per-frame director update driven by the HLTV server; flDelay becomes the value GetDelay() returns.
	virtual void	UpdateDirector( float flDelay ) = 0;
};

#endif // IHLTVDIRECTOR_H
