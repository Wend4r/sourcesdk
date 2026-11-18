#ifndef CSTRIKE15_WEAPON_CSBASEGUN_H
#define CSTRIKE15_WEAPON_CSBASEGUN_H

#pragma once

#include "weapon_csbase.h"

//-----------------------------------------------------------------------------
// CCSWeaponBaseGun adds 2 methods to CCSWeaponBase: Linux slots 448-449, Windows 437-438.
//-----------------------------------------------------------------------------
template < class TBase = CCSWeaponBaseVTable<> >
abstract_class CCSWeaponBaseGunVTable : public TBase
{
public:
	// Returns false; CWeaponTaser fires its own shot (Weapon_Taser.Single, the taser rope) and answers true, and the fire routine of the gun (CSBaseGunFire) then skips the bullets. Named by what it does
	virtual bool FireCustomShot( const void *pShotInfo, int nUnknown, int nMode ) = 0;
	// Returns IsSilenced; never overridden, the same body in C_CSWeaponBaseGun (client slot 443). No caller on the server or the client. Named by what it does
	virtual bool IsSilencedGun() = 0;
};

#endif // CSTRIKE15_WEAPON_CSBASEGUN_H
