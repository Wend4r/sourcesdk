#ifndef CSTRIKE15_BASECSGRENADE_PROJECTILE_H
#define CSTRIKE15_BASECSGRENADE_PROJECTILE_H

#pragma once

#include "../basegrenade_shared.h"

class CGameTrace;

//-----------------------------------------------------------------------------
// CBaseCSGrenadeProjectile adds 5 methods to CBaseGrenade: Linux slots 320-324, Windows 319-323.
//-----------------------------------------------------------------------------
template < class TBase = CBaseGrenadeVTable<> >
abstract_class CBaseCSGrenadeProjectileVTable : public TBase
{
public:
	virtual void DangerSoundThink() = 0;
	// CSmokeGrenadeProjectile overrides
	virtual void OnBounced() = 0;
	// The bounce response of the projectile, with network state changes; not the ResolveFlyCollisionCustom of CBaseEntity, which the class overrides as well
	virtual void Unk_321( CGameTrace &trace, Vector &vecMove ) = 0;
	// Zeroes the networked m_flDetonateTime when set; CHEGrenadeProjectile overrides it with an empty body. Named by what it does
	virtual void ClearDetonateTime() = 0;
	// Unless m_bHasEverHitEnemy, traces along the velocity for players of the other team than the thrower. Named by what it does
	virtual bool CheckForEnemyHit() = 0;
};

#endif // CSTRIKE15_BASECSGRENADE_PROJECTILE_H
