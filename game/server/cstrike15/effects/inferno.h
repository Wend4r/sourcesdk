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

#endif // CSTRIKE15_INFERNO_H
