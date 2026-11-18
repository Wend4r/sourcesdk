#ifndef CSTRIKE15_CS_PLAYER_ITEMSERVICES_H
#define CSTRIKE15_CS_PLAYER_ITEMSERVICES_H

#pragma once

#include "../player_itemservices.h"

//-----------------------------------------------------------------------------
// CCSPlayer_ItemServices adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_ItemServicesVTable<> >
abstract_class CCSPlayer_ItemServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_ITEMSERVICES_H
