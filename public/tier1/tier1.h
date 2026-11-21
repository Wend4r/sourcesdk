//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef TIER1_H
#define TIER1_H

#if defined( _WIN32 )
#pragma once
#endif

#include "appframework/iappsystem.h"
#include "tier0/convar.h"
#include "appframework/tier1app.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

struct ResourceManifestDesc_t;

//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 1 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
// Same as ConnectInterfaces() / DisconnectInterfaces(); kept for the application frameworks.
//-----------------------------------------------------------------------------
void ConnectTier1Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectTier1Libraries();

//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier1 libraries
//
// Connecting the interface globals and the ConVar registration are done by
// CTier0AppSystem; this level only reports its tier.
//-----------------------------------------------------------------------------
template< class IInterface, int64 ConVarFlag = 0 >
class CTier1AppSystem : public CTier0AppSystem< IInterface, ConVarFlag >
{
	typedef CTier0AppSystem< IInterface, ConVarFlag > BaseClass;

public:
	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER1;
	}
};

class CTier1Application : public CTier1AppSystem< IApplication >
{
public:
	virtual ~CTier1Application() = default;
};

#endif // TIER1_H

