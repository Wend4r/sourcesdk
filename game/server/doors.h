#ifndef DOORS_H
#define DOORS_H

#pragma once

#include "basetoggle.h"
#include "igameevents.h"

//-----------------------------------------------------------------------------
// CBaseDoor adds 3 methods to CBaseToggle, Windows slots 271-273.
// The Linux table has 4, slots 272-275: 1 override methods of IGameEventListener2, which
// the Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseToggleVTable<> >
abstract_class CBaseDoorVTable : public TBase, public IGameEventListener2
{
public:
	// Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
	// CRotDoor overrides (angles instead of origin)
	virtual void SetToggleState( int state ) = 0;
	// CBaseDoor returns false, CRotDoor true
	virtual bool IsRotatingDoor() const = 0;
	virtual void StopMovingSound() = 0;
};

#endif // DOORS_H
