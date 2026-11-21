//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Holds the definitions of the CS ammo types
//
//=============================================================================//

#ifndef CS_AMMODEF_H
#define CS_AMMODEF_H
#ifdef _WIN32
#pragma once
#endif

#include "ammodef.h"

//-----------------------------------------------------------------------------
// The ammo definition of CS. A global object; it adds no members.
//-----------------------------------------------------------------------------
class CCSAmmoDef : public CAmmoDefTyped< GameAmmoTypeInfo_t >
{
public:
	~CCSAmmoDef() override;

	// Adds the CS ammo types (BULLET_PLAYER_50AE ... AMMO_TYPE_HEALTHSHOT) with their buy size,
	// splash size, impulse convar and carry limit convar
	void Init() override;

	// The value of the ammo type's m_MaxCarryCVar when it is set, m_nMaxCarry otherwise
	int MaxCarry( AmmoIndex_t nAmmoIndex ) override;

	// Adds or replaces the ammo type of this name (FindOrAddAmmoType); warns
	// "CCSAmmoDef::AddAmmoType (%s) found no CVar named (%s)" for an unknown carry limit convar.
	// The impulse is stored as the range [ flPhysicsForceImpulse, flPhysicsForceImpulse ].
	// Init passes 0 for nCost on every type (the Linux build drops that argument).
	void AddAmmoType( const char *pszName, int nCost, int nBuySize, const char *pszMaxCarryCVar, float flPhysicsForceImpulse,
		AmmoFlags_t nFlags, int nMinSplashSize, int nMaxSplashSize );
};

// Get the global ammodef object as the CS ammo definition
CCSAmmoDef *GetCSAmmoDef();

#endif // CS_AMMODEF_H
