#ifndef CSTRIKE15_WEAPON_BASEITEM_H
#define CSTRIKE15_WEAPON_BASEITEM_H

#pragma once

#include "weapon_csbase.h"

class CCSPlayerPawn;

//-----------------------------------------------------------------------------
// CWeaponBaseItem adds 5 methods to CCSWeaponBase: Linux slots 448-452, Windows 437-441.
//-----------------------------------------------------------------------------
template < class TBase = CCSWeaponBaseVTable<> >
abstract_class CWeaponBaseItemVTable : public TBase
{
public:
	// Returns true in every class; the base weapon class no longer has the method
	virtual bool HasPrimaryAmmo() = 0;
	// Returns false in every class
	virtual bool CanFidget() = 0;
	// CItem_Healthshot refuses at full health (#SFUI_Healthshot_AlreadyAtMax)
	virtual bool CanUseOnSelf( CCSPlayerPawn *pPlayer ) = 0;
	// CItem_Healthshot overrides
	virtual void OnStartUse( CCSPlayerPawn *pPlayer ) = 0;
	// CItem_Healthshot heals
	virtual void CompleteUse( CCSPlayerPawn *pPlayer ) = 0;
};

#endif // CSTRIKE15_WEAPON_BASEITEM_H
