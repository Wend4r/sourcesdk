#ifndef CSTRIKE15_CS_OBSERVER_USESERVICES_H
#define CSTRIKE15_CS_OBSERVER_USESERVICES_H

#pragma once

#include "../player_useservices.h"

//-----------------------------------------------------------------------------
// CCSObserver_UseServices adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_UseServicesVTable<> >
abstract_class CCSObserver_UseServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_OBSERVER_USESERVICES_H
