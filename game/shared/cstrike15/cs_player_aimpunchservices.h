#ifndef CSTRIKE15_CS_PLAYER_AIMPUNCHSERVICES_H
#define CSTRIKE15_CS_PLAYER_AIMPUNCHSERVICES_H

#pragma once

#include "../playerpawn_component.h"

//-----------------------------------------------------------------------------
// CCSPlayer_AimPunchServices derives straight from
// CPlayerPawnComponent and adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CCSPlayer_AimPunchServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_AIMPUNCHSERVICES_H
