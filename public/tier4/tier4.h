#ifndef TIER4_H
#define TIER4_H

#pragma once

#include "tier3/tier3.h"


//-----------------------------------------------------------------------------
// Call this to connect to/disconnect from all tier 3 libraries.
// It's up to the caller to check the globals it cares about to see if ones are missing
//-----------------------------------------------------------------------------
void ConnectTier4Libraries( CreateInterfaceFn *pFactoryList, int nFactoryCount );
void DisconnectTier4Libraries();


//-----------------------------------------------------------------------------
// Helper empty implementation of an IAppSystem for tier3 libraries
//-----------------------------------------------------------------------------
template< class IInterface, int ConVarFlag = 0 > 
class CTier4AppSystem : public CTier3AppSystem< IInterface, ConVarFlag >
{
	typedef CTier3AppSystem< IInterface, ConVarFlag > BaseClass;

public:
	virtual bool Connect( CreateInterfaceFn factory ) 
	{
		if ( !BaseClass::Connect( factory ) )
			return false;

		ConnectTier4Libraries( &factory, 1 );
		return true;
	}

	virtual void Disconnect() 
	{
		DisconnectTier4Libraries();
		BaseClass::Disconnect();
	}

	virtual AppSystemTier_t GetTier()
	{
		return APP_SYSTEM_TIER4;
	}
};

#endif // TIER4_H
