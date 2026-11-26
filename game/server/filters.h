#ifndef FILTERS_H
#define FILTERS_H

#pragma once

#include "baseentity.h"

class CTakeDamageInfo;

//-----------------------------------------------------------------------------
// CBaseFilter adds 2 methods to CLogicalEntity (through CLogicalEntity and CServerOnlyEntity, which add no method): Linux slots 239-240, Windows 238-239.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CBaseFilterVTable : public TBase
{
public:
	virtual bool PassesFilterImpl( CBaseEntity *pCaller, CBaseEntity *pEntity ) = 0;
	virtual bool PassesDamageFilterImpl( const CTakeDamageInfo &info ) = 0;
};

#endif // FILTERS_H
