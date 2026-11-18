#ifndef CSTRIKE15_CS_PLAYER_ACTIONTRACKINGSERVICES_H
#define CSTRIKE15_CS_PLAYER_ACTIONTRACKINGSERVICES_H

#pragma once

#include "../playerpawn_component.h"

//-----------------------------------------------------------------------------
// CCSPlayer_ActionTrackingServices derives straight from
// CPlayerPawnComponent and adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CCSPlayer_ActionTrackingServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_ACTIONTRACKINGSERVICES_H
