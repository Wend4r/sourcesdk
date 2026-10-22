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

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "tier0/utlvector.h"
#include "nav_area.h"
#include "nav_ladder.h"

class INavPathCost;
class CNavPathCost;
class CNavPath;
class CPathOptimizer;
struct NavTransition_t;

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
		// The game also uses 7 for a segment in a nav link area.
	};

	// NavTraverseType; GO_SAME_AREA for the first segment.
	int how;
	// SegmentType.
	int type;
	// The edge of the previous area crossed into this one, -1 for the first segment.
	int8 inEdge;
	// The ladder of a ladder segment.
	const CNavLadder *pLadder;

	// Distance to the next segment's position; 0 for the last segment.
	float length;
	// Sum of the lengths of the previous segments.
	float distanceFromStart;
	// 0 for the only segment of a one-segment path.
	float m_flUnk20;
	// FLT_MAX when the lengths are computed.
	float m_flUnk24;
	// Bit 0: the step failed the cost check and must not be optimized away.
	int m_nFlags;
	// The start position for the first segment; the point the segment is entered at for the rest.
	Vector vPos;
	// Index of the area in the active mesh's m_NavAreas.
	int m_nAreaIndex;
	uint32 m_nAreaId;
	// -1 for the first segment.
	int m_nPrevAreaIndex;

private:
	int m_nUnk44;
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
	// Set once the path went through the portal optimization and m_pOptimizer, so neither runs twice; cleared by
	// InvalidatePath
	bool m_bOptimized;

private:
	uint8 m_Unk480D[ 3 ];

public:
	// IDs of the nav link areas (dynamic attribute 0x200) the path goes through, whose segments
	// get type 7.
	CUtlVectorFixedGrowable< uint32, 2 > m_Unk4810;

	int m_PathResultStatus;
	bool m_bUnk4834;
};

//-----------------------------------------------------------------------------
// A path with the query it was computed for. Adds no virtuals of its own.
//-----------------------------------------------------------------------------
abstract_class CNavPath : public CNavPathData
{
public:
	// Copied from the request each time a path is computed.
	struct Query_t
	{
		bool m_bUnk0;
		void *m_pUnk8;
		const INavPathCost *m_pPathCost;
		// -1 until set.
		float m_flUnk18;
		float m_flUnk1C;
		// 1 until set.
		float m_flUnk20;
		float m_flUnk24;
		bool m_bUnk28;
	};

	const INavPathCost *GetPathCost() const { return m_Query.m_pPathCost; }

public:
	Query_t m_Query;
	// Passed to the goal area search; true until set.
	bool m_bUnk4868;
	// Run over computed paths when set, unless disabled by a convar.
	CPathOptimizer *m_pOptimizer;
	// Null at construction and never set by the game. When set, slot 0 is called with the
	// path and m_Query.m_pPathCost right after the segments are post-processed, like
	// m_pOptimizer later.
	void *m_pUnk4878;
};

//-----------------------------------------------------------------------------
// Straightens a computed path in place.
//-----------------------------------------------------------------------------
abstract_class CPathOptimizer
{
public:
	virtual void OptimizePath( CNavPath &path, const CNavPathCost &cost ) = 0;
	virtual ~CPathOptimizer() = 0;

public:
	bool m_bUnk8;

private:
	uint8 m_Unk09[ 7 ];
	// Filled from the transitions FindNextOccludedNode collects.
	CUtlVector< NavTransition_t > m_Unk10;
};

//-----------------------------------------------------------------------------
// Removes the segments a ray over the mesh can skip.
//-----------------------------------------------------------------------------
abstract_class CPathOptimizerNavmesh : public CPathOptimizer
{
protected:
	virtual int FindNextOccludedNode( CNavPath &path, const CNavPathCost &cost, int nNode, CUtlVectorFixedGrowable< NavTransition_t, 16 > *pTransitions ) = 0;

private:
	// A std::function, whose size differs between the two standard libraries.
#ifdef _WIN32
	uint8 m_Unk28[ 0x40 ];
#else
	uint8 m_Unk28[ 0x20 ];
#endif
};

#endif // NAV_PATH_H
