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

#define INTERFACEVERSION_HLTVDIRECTOR			"HLTVDirector001"

class IHLTVDirector
{
public:
	virtual	~IHLTVDirector() {}

	virtual bool	IsActive( void ) = 0; // true if director is active

	virtual void AddHLTVServer( IHLTVServer *hltv ) = 0; // give the director the engine HLTV interface 
	virtual void RemoveHLTVServer( IHLTVServer *hltv ) = 0;

	virtual IHLTVServer* GetHLTVServer( int instance ) = 0; // get HLTV server interface of instance
	virtual int GetHLTVServerCount( void ) = 0;
	
	virtual int		GetDirectorTick( void ) = 0;	// get current broadcast tick from director
	virtual int		GetPVSEntity( void ) = 0; // get current view entity (PVS), 0 if coords are used
	virtual Vector	GetPVSOrigin( void ) = 0; // get current PVS origin
	virtual float	GetDelay( void ) = 0; // returns current delay in seconds

	virtual const char**	GetModEvents() = 0;
};

#endif // IHLTVDIRECTOR_H
