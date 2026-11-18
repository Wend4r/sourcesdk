#ifndef CSTRIKE15_CS_PLAYER_CONTROLLER_INGAMEMONEYSERVICES_H
#define CSTRIKE15_CS_PLAYER_CONTROLLER_INGAMEMONEYSERVICES_H

#pragma once

#include "../playercontroller_component.h"

//-----------------------------------------------------------------------------
// CCSPlayerController_InGameMoneyServices adds no virtual method. See
// playercontroller_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerControllerComponentVTable >
abstract_class CCSPlayerController_InGameMoneyServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_CONTROLLER_INGAMEMONEYSERVICES_H
