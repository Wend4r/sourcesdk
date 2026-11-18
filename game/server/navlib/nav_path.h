//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: A computed path over the navigation mesh and the optimizers that smooth it.
//
//=============================================================================//

#ifndef NAV_PATH_H
#define NAV_PATH_H

#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "tier0/utlvector.h"
#include "nav_area.h"
#include "nav_ladder.h"

class CBaseEntity;
class INavPathCost;
class CNavPathCost;
class CNavPath;
class CPathOptimizer;
class CNavPathJumpProcess;
struct NavAreaAdj_t;

//-----------------------------------------------------------------------------
// One step of a path: the area it is in and how it is entered from the previous one.
//-----------------------------------------------------------------------------
struct CPathSegment
{
	enum SegmentType
	{
		PATH_SEG_ON_GROUND = 0,
		PATH_SEG_JUMP_DOWN = 1,
		PATH_SEG_DROP_DOWN = 2,
		PATH_SEG_JUMP_OVER_GAP = 3,
		PATH_SEG_CLIMB_UP = 4,
		PATH_SEG_LADDER_UP = 5,
		PATH_SEG_LADDER_DOWN = 6,
		// The game also uses 7 for a segment in a nav link area and 10 for the coarse segments the
		// path stitcher of the nav test tools keeps.
	};

	// NavTraverseType; GO_SAME_AREA for the first segment.
	int how;
	// SegmentType.
	int type;
	// The edge of the previous area crossed into this one, -1 for the first segment.
	int8 inEdge;
	// The ladder of a ladder segment.
	const CNavLadder *pLadder;
	// The distance to the next segment's position, and the sum of the lengths before this
	// segment; both computed once the path is post-processed.
	float flLength;
	float flDistanceFromStart;
	// Named after the Dota 2017 CPathSegment members that follow the two lengths. This build
	// only copies flCurvature; the length pass resets flPortalHalfWidth to FLT_MAX.
	float flCurvature;
	float flPortalHalfWidth;
	// Bit 0: the step failed the cost check and must not be optimized away.
	int m_nFlags;
	// The start position for the first segment; the point the segment is entered at for the rest.
	Vector vPos;
	// Index of the area in the active mesh's m_NavAreas.
	int m_nAreaIndex;
	uint32 m_nAreaId;
	// -1 for the first segment.
	int m_nPrevAreaIndex;
	// Named by what it does: -1 until the path stitcher of the nav test tools assigns it to a
	// segment of type 10; the stitcher keeps it while it rebuilds the path.
	int m_nStitchedNodeId;
};

//-----------------------------------------------------------------------------
// The segments of a path and how it ended.
//-----------------------------------------------------------------------------
abstract_class CNavPathData
{
public:
	enum PathResult_t
	{
		NO_PATH = 0,
		PATH_COMPLETE_TO_GOAL = 1,
		PATH_COMPLETE_TO_NEAREST_AREA = 2,
		PATH_PARTIAL = 3,
	};

	enum { MAX_PATH_SEGMENTS = 256 };

	virtual ~CNavPathData() = 0;
	// Drops every segment.
	virtual void InvalidatePath() = 0;
	virtual void Draw( const CPathSegment *pStart, float flDuration ) const = 0;

	bool IsValid() const { return m_nSegmentCount > 0; }
	int GetSegmentCount() const { return m_nSegmentCount; }
	const CPathSegment &GetSegment( int i ) const { return m_path[ i ]; }
	PathResult_t GetResult() const { return ( PathResult_t )m_PathResultStatus; }

public:
	CPathSegment m_path[ MAX_PATH_SEGMENTS ];
	int m_nSegmentCount;
	// Named by what it does: set once the segments have been smoothed and run through the
	// optimizer, cleared by InvalidatePath.
	bool m_bPostProcessed;
	// The m_nID of every area of the path with NAV_AREA_NAV_LINK, in path order.
	CUtlVectorFixedGrowable< uint32, 2 > m_NavLinkAreaIds;
	int m_PathResultStatus;
	// Named by what it does: set by the path stitcher of the nav test tools, cleared by
	// InvalidatePath.
	bool m_bStitched;
};

//-----------------------------------------------------------------------------
// A path with the query it was computed for. Adds no virtuals of its own.
//-----------------------------------------------------------------------------
abstract_class CNavPath : public CNavPathData
{
public:
	// Copied from the request each time a path is computed: the CS2 form of the Dota 2017
	// PathCapabilities_t together with the limits Dota kept in CPath.
	struct Query_t
	{
		// Draws the drop-down and detail passes.
		bool m_bIsDebugging;
		// The entity the drop-down traces skip.
		CBaseEntity *m_pPathingEntity;
		const INavPathCost *m_pPathCost;
		// The search gives up past this cost and the path is cut at this length; -1 for no limit.
		float m_flMaxPathLength;
		// Named by what it does: areas farther than this from the straight line to the goal are
		// not searched; -1 for no limit.
		float m_flMaxDetourDistance;
		// A step whose next point lies more than this below becomes a drop-down. 1 until set.
		float m_flMoverStepHeight;
		// The height of the hull traced for drop-downs. 1 until set.
		float m_flMoverCrouchHullHeight;
		// A partial path counts as a success.
		bool m_bAllowPartialPaths;
	};

	const INavPathCost *GetPathCost() const { return m_Query.m_pPathCost; }

public:
	Query_t m_Query;
	// Named by what it does: makes the goal area search stop one grid ring after its first
	// candidate. True until set.
	bool m_bQuickGoalAreaSearch;
	// Run over computed paths when set, unless disabled by a convar.
	CPathOptimizer *m_pOptimizer;
	// Called with the path before the optimizer; never set in CS2. Name from the Dota 2017 CPath.
	CNavPathJumpProcess *m_pJumpPostProcess;
};

//-----------------------------------------------------------------------------
// A step of a path recorded by an optimizer: where it is entered, the area, how and through
// which edge. Named by what it holds; the type name is the one the optimizer's signature uses.
//-----------------------------------------------------------------------------
struct NavTransition_t
{
	Vector m_vPos;
	CNavArea *m_pArea;
	// NavTraverseType.
	int16 m_nHow;
	int16 m_nEdge;
};

//-----------------------------------------------------------------------------
// Straightens a computed path in place. The base does nothing.
//-----------------------------------------------------------------------------
class CPathOptimizer
{
public:
	virtual void OptimizePath( CNavPath &path, const CNavPathCost &cost ) {}
	virtual ~CPathOptimizer() {}

public:
	// Named by what it does: OptimizePath records every step it keeps into m_Transitions.
	bool m_bRecordTransitions;
	CUtlVector< NavTransition_t > m_Transitions;
};

// A transition filter of the ray tests over the mesh: whether the ray may go from an area
// through a connection, given the segment's start and end.
typedef std::function< bool( const CNavArea *, const NavAreaAdj_t *, const Vector &, const Vector &, bool ) > FuncNavAreaTransition_t;

//-----------------------------------------------------------------------------
// Removes the segments a ray over the mesh can skip.
//-----------------------------------------------------------------------------
abstract_class CPathOptimizerNavmesh : public CPathOptimizer
{
protected:
	virtual int FindNextOccludedNode( CNavPath &path, const CNavPathCost &cost, int nNode, CUtlVectorFixedGrowable< NavTransition_t, 16 > *pTransitions ) = 0;

public:
	// Typed as the transition filter the mesh ray test takes; only the destructor touches it in
	// this build.
	FuncNavAreaTransition_t m_fnTransitionFilter;
};

#endif // NAV_PATH_H
