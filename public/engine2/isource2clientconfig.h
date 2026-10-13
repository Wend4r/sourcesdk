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

struct RenderDeviceInfo_t;
enum RenderMultisampleType_t : uint8;
FORWARD_DECLARE_HANDLE( SwapChainHandle_t );

#define SOURCE2CLIENTCONFIG_INTERFACE_VERSION "Source2ClientConfig001"

abstract_class ISource2ClientConfig : public IAppSystem
{
public:
	virtual int GetMaxSplitscreenPlayers() = 0;
	virtual void AllocateScratchRenderTargets( SwapChainHandle_t hSwapChain, const RenderDeviceInfo_t &deviceInfo, RenderMultisampleType_t nMultisampleType ) = 0;
	virtual CUtlStringToken GetRenderingPipelineToken() = 0;
	virtual bool InstantReplayShouldSkipMessage( int nMessageId ) const = 0;
	// Asked for net message ids the engine has no handler for
	virtual bool InstantReplayShouldIgnoreMessage( int nMessageId ) const = 0;
	virtual bool IsCompatibleNetworkVersion( int nServerNetworkVersion ) = 0;
	virtual const char *ChangeWorldMountNameByNetworkVersion( const char *pszWorldName, int nServerNetworkVersion ) const = 0;
	virtual void *unk018() = 0;
};

#endif // ISOURCE2CLIENTCONFIG_H
