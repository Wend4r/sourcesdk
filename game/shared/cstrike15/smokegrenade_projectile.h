#ifndef CSTRIKE15_SMOKEGRENADE_PROJECTILE_H
#define CSTRIKE15_SMOKEGRENADE_PROJECTILE_H

#pragma once

#include "basecsgrenade_projectile.h"
#include "igameevents.h"

//-----------------------------------------------------------------------------
// CSmokeGrenadeProjectile adds no method to CBaseCSGrenadeProjectile. The Linux table has 1 more slot, 325: the override of a method of IGameEventListener2, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseCSGrenadeProjectileVTable<> >
abstract_class CSmokeGrenadeProjectileVTable : public TBase, public IGameEventListener2
{
public:
	// Listens to break_prop among others. Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
};

#endif // CSTRIKE15_SMOKEGRENADE_PROJECTILE_H
