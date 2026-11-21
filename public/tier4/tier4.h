#ifndef TIER4_H
#define TIER4_H

#pragma once

#include "tier3/tier3.h"


//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 4 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
// The tier 4 app systems of the game do not call them: every interface global
// is already connected by CTier0AppSystem.
//-----------------------------------------------------------------------------
void ConnectTier4Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectTier4Libraries();


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier4 libraries
//
// Unlike the lower tiers the ConVar flag parameter is an int here, and is
// converted when it is passed down.
//-----------------------------------------------------------------------------
template< class IInterface, int ConVarFlag = 0 >
class CTier4AppSystem : public CTier3AppSystem< IInterface, ConVarFlag >
{
	typedef CTier3AppSystem< IInterface, ConVarFlag > BaseClass;

public:
	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER4;
	}
};

#endif // TIER4_H
