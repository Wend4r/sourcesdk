#ifndef CSTRIKE15_CS_OBSERVER_MOVEMENTSERVICES_H
#define CSTRIKE15_CS_OBSERVER_MOVEMENTSERVICES_H

#pragma once

#include "../player_movementservices.h"

//-----------------------------------------------------------------------------
// CCSObserver_MovementServices adds no virtual method. It derives from
// CPlayer_MovementServices, not from the humanoid class. See playerpawn_component.h.
//-----------------------------------------------------------------------------
template < class TBase = CPlayer_MovementServicesVTable<> >
abstract_class CCSObserver_MovementServicesVTable : public TBase
{
};

#endif // CSTRIKE15_CS_OBSERVER_MOVEMENTSERVICES_H
