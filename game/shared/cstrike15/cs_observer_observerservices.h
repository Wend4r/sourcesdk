#ifndef CSTRIKE15_CS_OBSERVER_OBSERVERSERVICES_H
#define CSTRIKE15_CS_OBSERVER_OBSERVERSERVICES_H

#pragma once

#include "../player_observerservices.h"

//-----------------------------------------------------------------------------
// CCSObserver_ObserverServices adds no virtual method. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_ObserverServicesVTable<> >
abstract_class CCSObserver_ObserverServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_OBSERVER_OBSERVERSERVICES_H
