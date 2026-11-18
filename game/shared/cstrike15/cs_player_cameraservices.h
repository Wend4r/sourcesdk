#ifndef CSTRIKE15_CS_PLAYER_CAMERASERVICES_H
#define CSTRIKE15_CS_PLAYER_CAMERASERVICES_H

#pragma once

#include "../player_cameraservices.h"

//-----------------------------------------------------------------------------
// CCSPlayerBase_CameraServices, slot 33. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_CameraServicesVTable<> >
abstract_class CCSPlayerBase_CameraServicesVTable : public TBase
{
public:
	// Zero in the base class; CCSPlayer_CameraServices returns m_vecCsViewPunchAngle decayed to the current time
	virtual QAngle GetCsViewPunchAngle() = 0;
};

//-----------------------------------------------------------------------------
// CCSPlayer_CameraServices adds no virtual method.
//-----------------------------------------------------------------------------
template < class TBase = CCSPlayerBase_CameraServicesVTable<> >
abstract_class CCSPlayer_CameraServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_CAMERASERVICES_H
