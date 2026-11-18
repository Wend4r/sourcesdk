#ifndef CSTRIKE15_CS_PLAYER_CONTROLLER_ACTIONTRACKINGSERVICES_H
#define CSTRIKE15_CS_PLAYER_CONTROLLER_ACTIONTRACKINGSERVICES_H

#pragma once

#include "../playercontroller_component.h"

//-----------------------------------------------------------------------------
// CCSPlayerController_ActionTrackingServices adds no virtual method. See
// playercontroller_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerControllerComponentVTable >
abstract_class CCSPlayerController_ActionTrackingServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_CONTROLLER_ACTIONTRACKINGSERVICES_H
