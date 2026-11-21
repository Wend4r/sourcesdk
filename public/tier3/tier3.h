//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef TIER3_H
#define TIER3_H

#if defined( _WIN32 )
#pragma once
#endif

#include "tier2/tier2.h"


//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 3 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
// The tier 3 app systems of the game do not call them: every interface global
// is already connected by CTier0AppSystem.
//-----------------------------------------------------------------------------
void ConnectTier3Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectTier3Libraries();


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier3 libraries
//-----------------------------------------------------------------------------
template< class IInterface, int64 ConVarFlag = 0 >
class CTier3AppSystem : public CTier2AppSystem< IInterface, ConVarFlag >
{
	typedef CTier2AppSystem< IInterface, ConVarFlag > BaseClass;

public:
	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER3;
	}
};


#endif // TIER3_H
