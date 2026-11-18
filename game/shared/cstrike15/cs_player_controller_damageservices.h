#ifndef CSTRIKE15_CS_PLAYER_CONTROLLER_DAMAGESERVICES_H
#define CSTRIKE15_CS_PLAYER_CONTROLLER_DAMAGESERVICES_H

#pragma once

#include "../playercontroller_component.h"

//-----------------------------------------------------------------------------
// CCSPlayerController_DamageServices adds no virtual method. See
// playercontroller_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerControllerComponentVTable >
abstract_class CCSPlayerController_DamageServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_CONTROLLER_DAMAGESERVICES_H
