//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Lookup of the econ data of a weapon by its item definition index.
//
//=============================================================================//

#ifndef ICSWEAPONSYSTEM_H
#define ICSWEAPONSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

// The econ data of one weapon item definition (80 bytes, one virtual method)
class CWeaponEconData;

// Implemented by CCSWeaponSystem. No source names the interface's method; it is named by what it
// does. No virtual destructor.
abstract_class ICSWeaponSystem
{
public:
	// Returns the cached econ data of the item definition (0xFFFF gives null), creating it when the
	// item schema changed or it is not cached yet; null when the definition has no weapon data
	virtual CWeaponEconData *GetWeaponEconData( uint16 nItemDefIndex ) = 0;
};

#endif // ICSWEAPONSYSTEM_H
