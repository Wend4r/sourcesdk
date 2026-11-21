//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filter of the rotating prop doors.
//
//=============================================================================//

#ifndef PROPS_H
#define PROPS_H
#ifdef _WIN32
#pragma once
#endif

#include "../shared/util_shared.h"

//-----------------------------------------------------------------------------
// Checks whether a rotating door can swing: weapons never block it, a dynamic physics object
// blocks it only when it weighs at least 32 kg. No fields of its own.
//-----------------------------------------------------------------------------
class CTraceFilterDoor : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterDoor( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup )
	{
	}

	bool ShouldHitEntity( CEntityInstance *pEntity ) override;
};

#endif // PROPS_H
