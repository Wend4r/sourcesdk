#ifndef CSTRIKE15_CS_OBSERVER_CAMERASERVICES_H
#define CSTRIKE15_CS_OBSERVER_CAMERASERVICES_H

#pragma once

#include "cs_player_cameraservices.h"

//-----------------------------------------------------------------------------
// CCSObserver_CameraServices adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CCSPlayerBase_CameraServicesVTable<> >
abstract_class CCSObserver_CameraServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_OBSERVER_CAMERASERVICES_H
