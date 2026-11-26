#ifndef CSTRIKE15_FUNC_HOSTAGE_RESCUE_H
#define CSTRIKE15_FUNC_HOSTAGE_RESCUE_H

#pragma once

#include "../triggers.h"

//-----------------------------------------------------------------------------
// CHostageRescueZoneShim adds 1 method to CBaseTrigger: Linux slot 280, Windows 279.
//-----------------------------------------------------------------------------
template < class TBase = CBaseTriggerVTable<> >
abstract_class CHostageRescueZoneShimVTable : public TBase
{
public:
	// CHostageRescueZone fires "OnRescueZoneTouch" on the toucher
	virtual void HostageRescueTouch( CBaseEntity *pOther ) = 0;
};

//-----------------------------------------------------------------------------
// CHostageRescueZone adds 1 method to CHostageRescueZoneShim: Linux slot 281, Windows 280.
//-----------------------------------------------------------------------------
template < class TBase = CHostageRescueZoneShimVTable<> >
abstract_class CHostageRescueZoneVTable : public TBase
{
public:
	// Removes the previous hint entity, finds the zone centre and creates an info_hostage_rescue_zone_hint. Named by what it does
	virtual bool SetupRescueZoneHint() = 0;
};

#endif // CSTRIKE15_FUNC_HOSTAGE_RESCUE_H
