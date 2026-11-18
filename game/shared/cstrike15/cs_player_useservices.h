#ifndef CSTRIKE15_CS_PLAYER_USESERVICES_H
#define CSTRIKE15_CS_PLAYER_USESERVICES_H

#pragma once

#include "../player_useservices.h"

//-----------------------------------------------------------------------------
// CCSPlayer_UseServices adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_UseServicesVTable<> >
abstract_class CCSPlayer_UseServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_USESERVICES_H
