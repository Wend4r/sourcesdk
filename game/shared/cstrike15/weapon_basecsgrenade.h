#ifndef CSTRIKE15_WEAPON_BASECSGRENADE_H
#define CSTRIKE15_WEAPON_BASECSGRENADE_H

#pragma once

#include "weapon_csbase.h"
#include "../basegrenade_shared.h"

class CBaseCSGrenadeProjectile;
class CCSPlayerPawn;
class QAngle;

//-----------------------------------------------------------------------------
// CBaseCSGrenade adds 6 methods to CCSWeaponBase: Linux slots 448-453, Windows 437-442.
//-----------------------------------------------------------------------------
template < class TBase = CCSWeaponBaseVTable<> >
abstract_class CBaseCSGrenadeVTable : public TBase
{
public:
	// Returns false; while m_bIsHeldByPlayer, the pin is not pulled and m_fThrowTime is not set, starts to pull the pin with m_flThrowStrength 1 (primary) or 0 (secondary). PrimaryAttack returns it with false, SecondaryAttack with true. Never overridden
	virtual bool Unk_437( bool bSecondary ) = 0;
	// Empty; CMolotovGrenade and CIncendiaryGrenade override it with other empty bodies
	virtual void Unk_438() = 0;
	virtual void ThrowGrenade() = 0;
	// Returns false; the decoy, the flashbang, the HE and the smoke grenade return true, the molotov and the incendiary keep false
	virtual bool DropLiveGrenadeWhenOwnerDies() = 0;
	// The same values as CBaseGrenade::GetGrenadeType of the projectiles (0 HE, 1 flashbang, 2 molotov, 3 decoy, 4 smoke). Named by what it does
	virtual GrenadeType_t GetGrenadeType() = 0;
	virtual CBaseCSGrenadeProjectile *EmitGrenade( Vector vecSrc, QAngle vecAngles, Vector vecVel, AngularImpulse angImpulse, CCSPlayerPawn *pPlayer ) = 0;
};

#endif // CSTRIKE15_WEAPON_BASECSGRENADE_H
