#ifndef CSTRIKE15_CS_PLAYER_BULLETSERVICES_H
#define CSTRIKE15_CS_PLAYER_BULLETSERVICES_H

#pragma once

#include "../playerpawn_component.h"

//-----------------------------------------------------------------------------
// CCSPlayer_BulletServices derives straight from
// CPlayerPawnComponent and adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayerPawnComponentVTable >
abstract_class CCSPlayer_BulletServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_BULLETSERVICES_H
