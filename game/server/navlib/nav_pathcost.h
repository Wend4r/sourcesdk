//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The cost functions pathfinding runs on.
//
//=============================================================================//

#ifndef NAV_PATHCOST_H
#define NAV_PATHCOST_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "schemasystem/schematypes.h"
#include "nav_area.h"
#include "nav_ladder.h"

//-----------------------------------------------------------------------------
// One step from an area to a neighbour: the neighbour, how it is reached (NavTraverseType),
// through which edge, and the ladder for ladder steps.
//-----------------------------------------------------------------------------
struct NavAreaAdj_t
{
	CNavArea *m_pArea;
	int m_nHow;
	int8 m_nEdge;
	CNavLadder *m_pLadder;
};

// The cost of one step and the position it ends at; a cost of -1 means impassable.
struct PathCostResult_t
{
	float m_flSegmentCost;
	Vector m_vPos;
};

//-----------------------------------------------------------------------------
// Schema class; the first slots describe it to the schema system.
//-----------------------------------------------------------------------------
abstract_class INavPathCost
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void Unk_1( void *p ) = 0;
	virtual void Unk_2( void *p ) = 0;
	virtual const char *Unk_GetClassName() const = 0;
	virtual ~INavPathCost() {}
	virtual void Unk_5( void *p ) = 0;
	// The first check of every step: whether pToArea may be entered from pFromArea along pAdj.
	// CNavPathCost only requires pToArea to be built for this cost's hull (or the hull to be -1).
	virtual bool IsToAreaTraversable( const CNavArea *pToArea, const CNavArea *pFromArea, const NavAreaAdj_t *pAdj ) const = 0;
	// CNavPathCost: a heap copy of this cost.
	virtual INavPathCost *Unk_Clone() const = 0;
	// CNavPathCost: whether pszClassName names this cost's class.
	virtual bool Unk_IsClass( const char *pszClassName ) const = 0;

public:
	NavHull_t m_navHull;
};

//-----------------------------------------------------------------------------
// The cost function of the navlib, and the base of the game's own ones.
//-----------------------------------------------------------------------------
abstract_class CNavPathCost : public INavPathCost
{
public:
	virtual void Unk_9( void *p ) = 0;
	virtual void Unk_10( void *p ) = 0;
	virtual void Unk_11( void *p ) = 0;
	virtual void Unk_12( void *p ) = 0;
	// Costs the step from pFromArea along pAdj into *pResult: -1 unless IsToAreaTraversable,
	// else CalcCostFloorAreas for floor steps and CalcCostLadder for ladder steps. pUnkPos and
	// bUnk are handed on to CalcCostFloorAreas; pUnk is not used for the cost.
	virtual void Unk_CalcCost( const CNavArea *pFromArea, const NavAreaAdj_t *pAdj, const void *pUnk, const Vector *pUnkPos, bool bUnk, PathCostResult_t *pResult ) const = 0;
	virtual void Unk_14( void *p ) = 0;
	// The distance between the positions plus m_flTransitionPenalty.
	virtual float Unk_TransitionCost( const void *pUnk1, const void *pUnk2, const Vector *pFrom, const Vector *pTo ) const = 0;
	// Returns flDist unchanged; game costs add their penalties here, and treat a height change
	// above their agent's climb as impassable.
	virtual float FloorAreaCost( const CNavArea *pArea, const CNavArea *pFromArea, float flDist, float flHeightChange ) const = 0;
	// The ladder's length, or -1 when ladders are not allowed.
	virtual float LadderCost( const CNavLadder *pLadder, const CNavArea *pToArea, const CNavArea *pFromArea ) const = 0;
	// Finds where the step from pFromPos crosses into pToArea through nEdge (or ends at pUnkPos
	// when it is given) and costs it with FloorAreaCost. With bUnk the cost is the plain
	// distance whenever FloorAreaCost allows the step.
	virtual void CalcCostFloorAreas( PathCostResult_t *pResult, const CNavArea *pFromArea, const Vector *pFromPos, const CNavArea *pToArea, int nHow, int8 nEdge, bool bUnk, const Vector *pUnkPos ) const = 0;
	// Ends the step at the ladder's top and costs it with LadderCost.
	virtual void CalcCostLadder( PathCostResult_t *pResult, const CNavArea *pFromArea, const CNavArea *pToArea, const CNavLadder *pLadder ) const = 0;

public:
	bool m_bAllowLadders;
	bool m_bCanFly;
	bool m_bCanSwim;
	float m_flWaterToGroundMaxHeight;
	float m_flGroundToWaterMaxHeight;
	float m_flGroundToWaterTransitionDistance;
	float m_flWaterToGroundTransitionDistance;
	float m_flFlyingTransitionTolerance;
	bool m_bOptimizeFlySpacePathfinds;
	bool m_bStringPullFlySpacePathfinds;
	bool m_bSupportsTransitions;
	float m_flTransitionPenalty;
};

#endif // NAV_PATHCOST_H
