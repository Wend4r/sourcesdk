//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Trace filter of the query cache.
//
//=============================================================================//

#ifndef QUERYCACHE_H
#define QUERYCACHE_H
#ifdef _WIN32
#pragma once
#endif

#include "util_shared.h"

//-----------------------------------------------------------------------------
// Hands every entity to an optional function of the cached query. 80 bytes on Windows, 72 on
// Linux.
//-----------------------------------------------------------------------------
class CTraceFilterQueryCache : public CTraceFilterSlowWithCallback
{
public:
	CTraceFilterQueryCache( uint64 nInteractsWith = 0, int nCollisionGroup = COLLISION_GROUP_DEFAULT, ShouldHitFunc_t pExtraShouldHitCheckFn = nullptr )
		: CTraceFilterSlowWithCallback( nInteractsWith, nCollisionGroup ), m_pExtraShouldHitCheckFunction( pExtraShouldHitCheckFn )
	{
	}

	// m_pExtraShouldHitCheckFunction( pEntity, m_nInteractsWith ), true without a function
	bool ShouldHitEntity( CEntityInstance *pEntity ) override;

public:
	ShouldHitFunc_t m_pExtraShouldHitCheckFunction;
};

#endif // QUERYCACHE_H
