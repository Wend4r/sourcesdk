//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: The baked lighting compiler ( lightmaps, light probe volumes ) driven by command scripts
//
//===========================================================================//

#ifndef IVRAD3_H
#define IVRAD3_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "appframework/iappsystem.h"

class CUtlBuffer;
class ISceneWorld;

abstract_class IVrad3 : public IAppSystem
{
public:
	// Tokenises the command line and runs the registered command named by its first token; an unknown command
	// is fatal
	virtual void ExecuteCommand( const char *pszCommandLine ) = 0;

	// The scene world "vrad3_raytrace_scene_world", created on first use
	virtual ISceneWorld *GetRaytraceSceneWorld() = 0;
	virtual void DestroyRaytraceSceneWorld() = 0;

	// Named by what they do: a map of numbered CUtlBuffer objects shared between commands. CreateBuffer returns
	// true when a buffer with that number already existed ( nothing is created then )
	virtual bool CreateBuffer( uint32 nBufferID ) = 0;
	virtual bool DestroyBuffer( uint32 nBufferID ) = 0;
	virtual CUtlBuffer *GetBuffer( uint32 nBufferID ) = 0;

	// Named by what they do: listeners that receive the tokenised arguments of every SendMessageToListeners;
	// AddMessageListener returns the id that RemoveMessageListener takes
	virtual int AddMessageListener( std::function< void( int nArgCount, const char **ppArgs ) > listener ) = 0;
	virtual void RemoveMessageListener( int nListenerID ) = 0;
	virtual void SendMessageToListeners( const char *pszMessage ) = 0;
};

#endif // IVRAD3_H
