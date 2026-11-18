#ifndef CSTRIKE15_CS_PLAYER_HOSTAGESERVICES_H
#define CSTRIKE15_CS_PLAYER_HOSTAGESERVICES_H

#pragma once

#include "../playerpawn_component.h"

//-----------------------------------------------------------------------------
// CCSPlayer_HostageServices derives straight from
// CPlayerPawnComponent and adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CCSPlayer_HostageServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_HOSTAGESERVICES_H
