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

#include "schemacompiler/schemaon.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNavBlock;

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
schema abstract_class INavPathCost
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual ~INavPathCost() {}
	virtual void unk005( void *p ) = 0;
	// The first check of every step: whether pToArea may be entered from pFromArea along pAdj.
	// CNavPathCost only requires pToArea to be built for this cost's hull (or the hull to be -1).
	virtual bool IsToAreaTraversable( const CNavArea *pToArea, const CNavArea *pFromArea, const NavAreaAdj_t *pAdj ) const = 0;
	// CNavPathCost: a heap copy of this cost.
	virtual INavPathCost *Clone() const = 0;
	// CNavPathCost: whether pszClassName names this cost's class.
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
	// Returns false in every cost of the game; no caller was found.
	virtual bool unk009( void *p ) = 0;
	// The first check of every step through fly or swim space: whether pToBlock may be entered,
	// from pFromBlock when it is given. CNavPathCost: false when the block has dynamic attribute
	// bit 2, true for a block of kind 1 with m_bCanFly or of kind 2 with m_bCanSwim (the low byte
	// of its attributes), false for the rest. Block queries given a cost skip the blocks it rejects.
	virtual bool IsToBlockTraversable( const CNavBlock *pToBlock, const CNavBlock *pFromBlock ) const = 0;
	// Whether the path may be smoothed across the step from pFromArea (reached at pFromPos)
	// along pAdj. The path optimizer's visibility walk rejects the steps it refuses, and
	// ComputePathDetails marks the segment so that the optimizer keeps it. True in every cost of
	// the game except the one approach areas are computed with.
	virtual bool unk011( const CNavArea *pFromArea, const Vector *pFromPos, const NavAreaAdj_t *pAdj ) const = 0;
	// Called by ComputePathDetails with the same arguments as unk011; true marks the segment so
	// that the optimizer keeps it. False in every cost of the game.
	virtual bool unk012( const CNavArea *pFromArea, const Vector *pFromPos, const NavAreaAdj_t *pAdj ) const = 0;
	// Costs the step from pFromArea, reached at pFromPos, along pAdj into *pResult: -1 unless
	// IsToAreaTraversable, else CalcCostFloorAreas for floor steps and CalcCostLadder for ladder
	// steps. The search records pFromPos as the position of pFromArea first; pGoalPos and
	// bDistanceOnly are handed on to CalcCostFloorAreas.
	virtual void CalcCost( const CNavArea *pFromArea, const NavAreaAdj_t *pAdj, const Vector *pFromPos, const Vector *pGoalPos, bool bDistanceOnly, PathCostResult_t *pResult ) const = 0;
	// Costs a step through fly or swim space from pFromBlock, reached at pFromPos, into the
	// block pAdj starts with, into *pResult: -1 unless IsToBlockTraversable allows it, 0 when
	// pFromPos is within 0.03125 of that block, else a cost computed from the portal bounds that
	// follow the block pointer in pAdj and from pGoalPos when it is given. pAdj is an element of
	// the space search's 56-byte neighbour list, whose type is not reconstructed.
	virtual void unk014( const CNavBlock *pFromBlock, const void *pAdj, const Vector *pFromPos, const Vector *pGoalPos, PathCostResult_t *pResult ) const = 0;
	// The cost of a transition between two areas (m_bSupportsTransitions): the distance between
	// the positions plus m_flTransitionPenalty. pUnk is the transition's transform object, which
	// the search uses to move positions between the two sides.
	virtual float TransitionCost( const CNavArea *pFromArea, const CNavArea *pToArea, const Vector *pFrom, const Vector *pTo, const Vector *pGoalPos, const void *pUnk ) const = 0;
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
