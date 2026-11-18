#ifndef CSTRIKE15_CS_PLAYER_WATERSERVICES_H
#define CSTRIKE15_CS_PLAYER_WATERSERVICES_H

#pragma once

#include "../player_waterservices.h"

//-----------------------------------------------------------------------------
// CCSPlayer_WaterServices adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_WaterServicesVTable<> >
abstract_class CCSPlayer_WaterServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_PLAYER_WATERSERVICES_H
