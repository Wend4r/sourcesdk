//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filter of the inferno (molotov fire).
//
//=============================================================================//

#ifndef CSTRIKE15_INFERNO_H
#define CSTRIKE15_INFERNO_H
#ifdef _WIN32
#pragma once
#endif

#include "../../../shared/util_shared.h"

//-----------------------------------------------------------------------------
// Line of sight of the fire: chickens (CImprovLocomotor) and glass traps do not block it
//-----------------------------------------------------------------------------
class CInfernoLOSTraceFilter : public CTraceFilterSlowWithCallback
{
public:
	CInfernoLOSTraceFilter( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

// BEGIN ENTITY VTABLES: generated, do not edit

#include "../../../shared/basemodelentity.h"

//-----------------------------------------------------------------------------
// CInferno adds 9 methods to CBaseModelEntity: Linux slots 272-280, Windows 271-279.
//-----------------------------------------------------------------------------
template < class TBase = CBaseModelEntityVTable<> >
abstract_class CInfernoVTable : public TBase
{
public:
	virtual int GetDamageType() = 0;
	virtual float GetDamagePerSecond() = 0;
	virtual const char *GetParticleEffectName() = 0;
	virtual const char *GetImpactParticleEffectName() = 0;
	virtual float GetFlameLifetime() const = 0;
	// 0.1 divided by the larger of a molotov or incendiary convar and 0.1, 1 once there are more than 63 flames; asked by InfernoThink when the fire spreads
	virtual float Unk_276() = 0;
	// A float convar of the molotov or the incendiary; asked by the burn damage
	virtual float Unk_277() = 0;
	virtual bool CanHarm( CBaseEntity *pEntity ) const = 0;
	virtual float GetFlameSpreadDelay() = 0;
};

// END ENTITY VTABLES

#endif // CSTRIKE15_INFERNO_H
