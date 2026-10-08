//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Game specific client configuration queried by the engine
//
//===========================================================================//

#ifndef ISOURCE2CLIENTCONFIG_H
#define ISOURCE2CLIENTCONFIG_H

#ifdef _WIN32
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "tier0/utlstringtoken.h"

#define SOURCE2CLIENTCONFIG_INTERFACE_VERSION "Source2ClientConfig001"

abstract_class ISource2ClientConfig : public IAppSystem
{
public:
	virtual int GetMaxSplitscreenPlayers() = 0;
	virtual void Unk_SetupRenderingPipeline( void *p ) = 0;
	virtual CUtlStringToken GetRenderingPipelineToken() = 0;
	virtual bool unk014( int nMessageId ) = 0;
	// Asked for net message ids the engine has no handler for
	virtual bool unk015( int nMessageId ) = 0;
	virtual bool IsCompatibleNetworkVersion( int nServerNetworkVersion ) = 0;
	virtual const char *unk017( const char *pszWorldName, int nServerNetworkVersion ) = 0;
	virtual void *unk018() = 0;
};

#endif // ISOURCE2CLIENTCONFIG_H
