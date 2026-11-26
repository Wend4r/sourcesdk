#ifndef LOGICENTITIES_H
#define LOGICENTITIES_H

#pragma once

#include "baseentity.h"
#include "igameevents.h"

//-----------------------------------------------------------------------------
// CLogicEventListener adds no method to CLogicalEntity (through CLogicalEntity and CServerOnlyEntity, which add no method). The Linux table has 1 more slot, 239: the override of a method of IGameEventListener2, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CLogicEventListenerVTable : public TBase, public IGameEventListener2
{
public:
	// Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
};

//-----------------------------------------------------------------------------
// CLogicGameEventListener adds no method to CLogicalEntity (through CLogicalEntity and CServerOnlyEntity, which add no method). The Linux table has 1 more slot, 239: the override of a method of IGameEventListener2, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CLogicGameEventListenerVTable : public TBase, public IGameEventListener2
{
public:
	// Overrides IGameEventListener2: a slot of this table on Linux only
	virtual void FireGameEvent( IGameEvent *pEvent ) override = 0;
};

//-----------------------------------------------------------------------------
// CLogicGameStateReport adds 2 methods to CBaseEntity: Linux slots 239-240, Windows 238-239.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CLogicGameStateReportVTable : public TBase
{
public:
	// Empty in every class
	virtual void Stub_238() = 0;
	// Empty
	virtual void SetGameStateReportThink() = 0;
};

//-----------------------------------------------------------------------------
// CLogicNPCCounter adds 3 methods to CBaseEntity: Linux slots 239-241, Windows 238-240.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CLogicNPCCounterVTable : public TBase
{
public:
	// |vecPos - origin| <= m_flDistanceMax; overridden by the AABB/OBB counters. Named by what it does
	virtual bool IsWithinDistance( const Vector &vecPos ) = 0;
	// Prints Count 1..3 / Factor 1..3 overlays. Named by what it does
	virtual void DrawDebugCounts() = 0;
	// Empty in the base
	virtual void SetNPCCounterThink() = 0;
};

//-----------------------------------------------------------------------------
// CLogicNPCCounterAABB adds 1 method to CLogicNPCCounter: Linux slot 242, Windows 241.
//-----------------------------------------------------------------------------
template < class TBase = CLogicNPCCounterVTable<> >
abstract_class CLogicNPCCounterAABBVTable : public TBase
{
public:
	// AABB transforms the box by the entity transform, CLogicNPCCounterOBB only takes component min/max. Named by what it does
	virtual void ComputeWorldBounds( const Vector &vecMins, const Vector &vecMaxs, Vector *pWorldMins, Vector *pWorldMaxs ) = 0;
};

#endif // LOGICENTITIES_H
