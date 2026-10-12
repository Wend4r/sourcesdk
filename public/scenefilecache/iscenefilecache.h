//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Loader of choreo scenes (.vcd resources)
//
//=============================================================================

#ifndef ISCENEFILECACHE_H
#define ISCENEFILECACHE_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"
#include "appframework/iappsystem.h"
#include "tier0/logging.h"

class CChoreoScene;
class IChoreoEventCallback;

// Values read from the root of a compiled scene resource
struct SceneCachedData_t
{
	// _scene_stop_time_ in milliseconds, rounded to nearest
	unsigned int	msecs;

	// _last_speak_time_ in seconds
	float			m_fLastSpeakSecs;

	// _has_speak_events_
	bool			m_bHasSpeakEvents;
};
COMPILE_TIME_ASSERT( sizeof( SceneCachedData_t ) == 0xC );

abstract_class ISceneFileCache : public IAppSystem
{
public:
	virtual bool unk011( const char *pszFile ) = 0;

	virtual CChoreoScene *LoadScene( const char *pszFile, IChoreoEventCallback *pCallback, LoggingChannelID_t nLogChannel, void ( *pfnPrintf )( const char *pszFormat, ... ) ) = 0;

	virtual bool GetSceneCachedData( const char *pszFile, SceneCachedData_t *pData ) = 0;
};

#define SCENE_FILE_CACHE_INTERFACE_VERSION "SceneFileCache002"

#endif // ISCENEFILECACHE_H
