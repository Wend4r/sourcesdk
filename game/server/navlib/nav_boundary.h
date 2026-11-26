//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The boundary of the navigation mesh generation (CNavBoundaryZone).
//
//=============================================================================//

#ifndef NAV_BOUNDARY_H
#define NAV_BOUNDARY_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "mathlib/vector.h"
#include "mathlib/vector2d.h"

//-----------------------------------------------------------------------------
// A boundary edge of the nav mesh: the piece of an area edge that has no neighbour. 28 bytes.
// Named by what it holds
//-----------------------------------------------------------------------------
struct NavBoundarySeg_t
{
	Vector m_vecStart;
	Vector m_vecEnd;
	// The index of the area edge the piece lies on
	int m_nEdge;
};

//-----------------------------------------------------------------------------
// A uniform 2D grid over boundary edges (at most 18 x 18 cells) that finds the edges near a point
// or along a ray (TestRay); below a size threshold the queries test every edge instead. 2016
// bytes. Not polymorphic; CNavBoundary embeds one and keeps a static default copy that restores
// the empty state.
//-----------------------------------------------------------------------------
class CNavSegGrid
{
public:
	// The edges of one cell: their number and where they start in m_ppCellSegments. Named by what it
	// holds
	struct Cell_t
	{
		uint16 m_nCount;
		uint16 m_nFirst;
		// The fill position while the grid is built
		uint16 m_nFilled;
	};

	// The grid's copy of an edge. 32 bytes. Named by what it holds
	struct GridSeg_t
	{
		NavBoundarySeg_t m_Seg;
		// The m_nQueryCount of the last query that visited the edge, so an edge in several cells is
		// visited once per query; -1 after building
		int m_nLastQuery;
	};

	enum { MAX_CELLS_PER_AXIS = 18 };

public:
	// Incremented by each query of the grid. Named by what it holds
	int m_nQueryCount;
	// The grid covers the edges' 2D bounds grown by a margin; the cells are square
	Vector2D m_vecMins;
	Vector2D m_vecMaxs;
	float m_flCellSize;
	int m_nCellsX;
	int m_nCellsY;
	int m_nSegmentCount;
	// The sum of the cell edge counts
	int m_nCellSegmentRefs;
	Cell_t m_Cells[ MAX_CELLS_PER_AXIS * MAX_CELLS_PER_AXIS ];
	// Heap array (delete[]) of the copies of the edges
	GridSeg_t *m_pSegments;
	// Heap array (delete[]) of pointers into m_pSegments, grouped by cell
	GridSeg_t **m_ppCellSegments;
	// The edges the grid was built from; the queries test them all when there are few
	int m_nSourceSegmentCount;
	const NavBoundarySeg_t *m_pSourceSegments;
};

//-----------------------------------------------------------------------------
// The boundary edges of a part of the nav mesh and a 2D grid over them. 2056 bytes. No virtual
// destructor.
//-----------------------------------------------------------------------------
abstract_class CNavBoundary
{
public:
	// Clears the boundary: empties its count, frees its arrays and copies the default settings
	// back; CNavBoundaryZone also empties its hash table. Named by what it does
	virtual void Reset() = 0;

public:
	// The boundary edges; the grid is built over them
	CUtlVector< NavBoundarySeg_t > m_Segments;
	CNavSegGrid m_Grid;
	// The smallest distance between two edges whose extents overlap within the given band, less
	// one and at least zero; computed over m_Segments and returned by a getter. Named by what it
	// holds
	float m_flMinSegmentGap;
};

#endif // NAV_BOUNDARY_H
