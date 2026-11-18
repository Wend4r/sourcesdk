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
#include "kv3lib/kv3transfer.h"
#include "nav_area.h"
#include "nav_ladder.h"

#include "schemacompiler/schemaon.h"

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
// Schema class; the first slots describe it to the schema system and transfer it to KV3.
//-----------------------------------------------------------------------------
schema abstract_class INavPathCost
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	CLASS_USES_KV3TRANSFER_VIRTUAL( INavPathCost );
	virtual const char *GetClassName() const = 0;
	virtual ~INavPathCost() {}
	// Named by what it does: empty in every cost class of the game and never called.
	virtual void EmptyVirtual() = 0;
	// The first check of every step: whether pToArea may be entered from pFromArea along pAdj.
	// CNavPathCost only requires pToArea to be built for this cost's hull (or the hull to be -1).
	virtual bool IsToAreaTraversable( const CNavArea *pToArea, const CNavArea *pFromArea, const NavAreaAdj_t *pAdj ) const = 0;
	// Named by what it does: a heap copy of this cost.
	virtual INavPathCost *Clone() const = 0;
	// Named by what it does: whether pszClassName names this cost's class.
	virtual bool IsClass( const char *pszClassName ) const = 0;

public:
	NavHull_t m_navHull;
};

//-----------------------------------------------------------------------------
// The cost function of the navlib, and the base of the game's own ones.
//-----------------------------------------------------------------------------
schema abstract_class CNavPathCost : public INavPathCost
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CNavPathCost );
	// Named by what it does: false in every cost class of the game (CNavPathCost, PathCost,
	// ChickenPathCost, HostagePathCost, ApproachAreaCost, CNavPathCostForTests); no caller found.
	virtual bool IsAlwaysFalse() const = 0;
	// Named by what it does: false for externally created areas; fly space areas need
	// m_bCanFly and water areas m_bCanSwim. pFromArea is not read by the navlib cost.
	virtual bool IsTransitionAllowed( const CNavArea *pToArea, const CNavArea *pFromArea ) const = 0;
	// Named by what they do: the path assembly keeps a step (sets its m_nFlags bit 0) unless
	// CanSkipStep allows skipping it, or when MustKeepStep asks for it. true and false in the
	// navlib cost; ApproachAreaCost returns false from CanSkipStep.
	virtual bool CanSkipStep( const CNavArea *pFromArea, const Vector *pFromPos, const NavAreaAdj_t *pAdj ) const = 0;
	virtual bool MustKeepStep( const CNavArea *pFromArea, const Vector *pFromPos, const NavAreaAdj_t *pAdj ) const = 0;
	// Named by what it does: costs the step from pFromArea along pAdj into *pResult: -1 unless
	// IsToAreaTraversable, else CalcCostFloorAreas for floor steps and CalcCostLadder for ladder
	// steps. pGoalPos and bDistanceOnly are handed on to CalcCostFloorAreas; pFromNode only goes
	// to a check whose result is discarded, and its type is not reconstructed.
	virtual void CalcCost( const CNavArea *pFromArea, const NavAreaAdj_t *pAdj, const void *pFromNode, const Vector *pGoalPos, bool bDistanceOnly, PathCostResult_t *pResult ) const = 0;
	// Named by what it does: unless IsTransitionAllowed rejects the transition's area, ends the
	// step at the point of the transition volume nearest its centre (at most 20 units from the
	// volume centre) and costs it by distance. pTransition is the area followed by the volume's
	// bounds; its type is not reconstructed.
	virtual void CalcTransitionCost( const CNavArea *pFromArea, const void *pTransition, const Vector *pFromPos, const Vector *pGoalPos, PathCostResult_t *pResult ) const = 0;
	// Named by what it does: the distance between the positions plus m_flTransitionPenalty.
	virtual float TransitionCost( const void *pFromTransition, const void *pToTransition, const Vector *pFrom, const Vector *pTo ) const = 0;
	// Returns flDist unchanged; game costs add their penalties here, and treat a height change
	// above their agent's climb as impassable.
	virtual float FloorAreaCost( const CNavArea *pArea, const CNavArea *pFromArea, float flDist, float flHeightChange ) const = 0;
	// The ladder's length, or -1 when ladders are not allowed.
	virtual float LadderCost( const CNavLadder *pLadder, const CNavArea *pToArea, const CNavArea *pFromArea ) const = 0;
	// Finds where the step from pFromPos crosses into pToArea through nEdge (or ends at pGoalPos
	// when it is given) and costs it with FloorAreaCost. With bDistanceOnly the cost is the
	// plain distance whenever FloorAreaCost allows the step.
	virtual void CalcCostFloorAreas( PathCostResult_t *pResult, const CNavArea *pFromArea, const Vector *pFromPos, const CNavArea *pToArea, int nHow, int8 nEdge, bool bDistanceOnly, const Vector *pGoalPos ) const = 0;
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

#include "schemacompiler/schemaoff.h"

#endif // NAV_PATHCOST_H
