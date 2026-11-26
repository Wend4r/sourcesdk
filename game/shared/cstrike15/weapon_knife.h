//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filter of the knife.
//
//=============================================================================//

#ifndef CSTRIKE15_WEAPON_KNIFE_H
#define CSTRIKE15_WEAPON_KNIFE_H
#ifdef _WIN32
#pragma once
#endif

#include "../util_shared.h"

//-----------------------------------------------------------------------------
// Skips the attacker (the first pass entity) and its teammates
//-----------------------------------------------------------------------------
class CTraceFilterKnifeIgnoreTeammates : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterKnifeIgnoreTeammates( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

// BEGIN ENTITY VTABLES: generated, do not edit

#include "weapon_csbase.h"

//-----------------------------------------------------------------------------
// CKnife adds 1 method to CCSWeaponBase: Linux slot 448, Windows 437.
//-----------------------------------------------------------------------------
template < class TBase = CCSWeaponBaseVTable<> >
abstract_class CKnifeVTable : public TBase
{
public:
	// Returns true in every class; the base weapon class no longer has the method
	virtual bool HasPrimaryAmmo() = 0;
};

// END ENTITY VTABLES

#endif // CSTRIKE15_WEAPON_KNIFE_H
