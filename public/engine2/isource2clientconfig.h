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
	// Dota 2 2018 names (CGameConfiguration / CSource1GameConfiguration)
	virtual void AllocateScratchRenderTargets( void *hSwapChain, const void *pDeviceInfo, int nMultisampleType ) = 0;
	virtual CUtlStringToken GetRenderingPipelineToken() = 0;
	virtual bool InstantReplayShouldSkipMessage( int nMessageId ) const = 0;
	// Asked for net message ids the engine has no handler for
	virtual bool InstantReplayShouldIgnoreMessage( int nMessageId ) const = 0;
	virtual bool IsCompatibleNetworkVersion( int nServerNetworkVersion ) = 0;
	// Returns pszWorldName in CS2
	virtual const char *ChangeWorldMountNameByNetworkVersion( const char *pszWorldName, int nServerNetworkVersion ) = 0;
	// AMNOTE: Empty in the client and the server of this build and not in the Dota 2018 table; no source names it
	virtual void EmptyClientConfigCallback() = 0;
};

#endif // ISOURCE2CLIENTCONFIG_H
