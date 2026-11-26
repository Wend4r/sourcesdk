#ifndef CSTRIKE15_HEGRENADE_PROJECTILE_H
#define CSTRIKE15_HEGRENADE_PROJECTILE_H

#pragma once

#include "basecsgrenade_projectile.h"

struct surfacedata_t;

//-----------------------------------------------------------------------------
// CHEGrenadeProjectile adds 2 methods to CBaseCSGrenadeProjectile: Linux slots 325-326, Windows 324-325.
//-----------------------------------------------------------------------------
template < class TBase = CBaseCSGrenadeProjectileVTable<> >
abstract_class CHEGrenadeProjectileVTable : public TBase
{
public:
	// Returns 5 in every class
	virtual int Stub_324() = 0;
	virtual const char *GetParticleSystemName( int pointContents, surfacedata_t *pData ) = 0;
};

#endif // CSTRIKE15_HEGRENADE_PROJECTILE_H
