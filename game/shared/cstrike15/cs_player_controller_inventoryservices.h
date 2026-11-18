#ifndef CSTRIKE15_CS_PLAYER_CONTROLLER_INVENTORYSERVICES_H
#define CSTRIKE15_CS_PLAYER_CONTROLLER_INVENTORYSERVICES_H

#pragma once

#include "../playercontroller_component.h"

//-----------------------------------------------------------------------------
// CCSPlayerController_InventoryServices adds no virtual method. See
// playercontroller_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerControllerComponentVTable >
abstract_class CCSPlayerController_InventoryServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_CONTROLLER_INVENTORYSERVICES_H
