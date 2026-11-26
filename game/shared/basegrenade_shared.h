#ifndef BASEGRENADE_SHARED_H
#define BASEGRENADE_SHARED_H

#pragma once

#include "baseanimgraph.h"

class CGameTrace;
class CTakeDamageInfo;

// The kind of a grenade
enum GrenadeType_t : uint32
{
	GRENADE_TYPE_EXPLOSIVE = 0,
	GRENADE_TYPE_FLASH = 1,
	GRENADE_TYPE_FIRE = 2,
	GRENADE_TYPE_DECOY = 3,
	GRENADE_TYPE_SMOKE = 4,
	GRENADE_TYPE_TOTAL = 5,
};

//-----------------------------------------------------------------------------
// CBaseGrenade adds 13 methods to CBaseAnimGraph: Linux slots 307-319, Windows 306-318.
//-----------------------------------------------------------------------------
template < class TBase = CBaseAnimGraphVTable<> >
abstract_class CBaseGrenadeVTable : public TBase
{
public:
	// Returns true in every class
	virtual bool IsActive() const = 0;
	virtual void Explode( CGameTrace *pTrace, int bitsDamageType ) = 0;
	virtual void BounceTouch( CBaseEntity *pOther ) = 0;
	virtual void Detonate() = 0;
	// Empty in every class
	virtual void Thrust() = 0;
	virtual void BounceSound() = 0;
	// Once only (sets m_bDamageDetonating), schedules the detonate think at now + flDelay and returns true; projectiles add their bounce sound / effects. Named by what it does
	virtual bool StartDamageDetonation( float flDelay ) = 0;
	// CBaseCSGrenadeProjectile overrides
	virtual float GetShakeAmplitude() = 0;
	virtual float GetShakeRadius() = 0;
	virtual GrenadeType_t GetGrenadeType() = 0;
	virtual float GetDamageRadius() = 0;
	virtual void SetDamageRadius( float flDamageRadius ) = 0;
	// Empty; Explode calls it with its trace and the damage info it built, right before SUB_Remove. CHEGrenadeProjectile plays the explosion effects (BaseGrenade.Explode, the explosion particles), CDecoyProjectile fires m_OnExplode with the attacker of the damage info as activator. Named by what it does
	virtual void OnExplode( CGameTrace *pTrace, const CTakeDamageInfo &info ) = 0;
};

#endif // BASEGRENADE_SHARED_H
