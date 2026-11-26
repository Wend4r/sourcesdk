//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Pathfinding helpers of the game side of the navigation mesh: the recorder of a
//          pathfind and the search wrapper. The pathfind test interface (IPathfindTest) is in
//          navlib/nav_pathfind.h. The include guard carries a prefix: navlib/nav_pathfind.h uses
//          NAV_PATHFIND_H.
//
//=============================================================================//

#ifndef SERVER_NAV_PATHFIND_H
#define SERVER_NAV_PATHFIND_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "navlib/nav_area.h"
#include "navlib/nav_pathfind.h"
#include "mathlib/vectorws.h"
#include "nav_search_bfs.h"

class CNavArea;
class CNavBlock;
class INavSearchNodeProcessor;

namespace NavRecorder
{

//-----------------------------------------------------------------------------
// Records the steps of a pathfind; the only implementation of the game is an empty one. No
// virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPathfindRecorder
{
public:
	virtual void Stub_0() = 0;
	virtual void Stub_1() = 0;
	virtual void Stub_2() = 0;
	virtual void Stub_3() = 0;
	virtual void Stub_4() = 0;
	virtual void Stub_5() = 0;
	virtual void Stub_6() = 0;
	virtual void Stub_7() = 0;
	virtual void Stub_8() = 0;
	virtual void Stub_9() = 0;
	virtual void Stub_10() = 0;
	virtual void Stub_11() = 0;
	virtual void Stub_12() = 0;
	virtual void Stub_13() = 0;
	virtual void Stub_14() = 0;
	virtual void Stub_15() = 0;
};

} // namespace NavRecorder

//-----------------------------------------------------------------------------
// Runs a lattice search of the nav mesh with a node processor it owns (CNavBFSLattice creates
// it): for every visited node the processor generates the lattice points around it and the
// wrapper hands each point to the callback of the node's kind until one returns
// NAV_SEARCH_HALT. 176 bytes on Linux, 272 on Windows (the std::function members differ in size).
//-----------------------------------------------------------------------------
abstract_class CNavSearchWrapper
{
public:
	virtual ~CNavSearchWrapper() = 0;

public:
	// The point callbacks, the first one set for the node's kind wins: area, block, then elem (for
	// any node). m_fnOnAreaPoint is a copy of the function the lattice is created with. Named by
	// what they do
	std::function< NavSearchResult_t( const VectorWS &, const CNavArea & ) > m_fnOnAreaPoint;
	std::function< NavSearchResult_t( const VectorWS &, const CNavBlock & ) > m_fnOnBlockPoint;
	std::function< NavSearchResult_t( const VectorWS &, const NavElemBase_t< 1 > & ) > m_fnOnElemPoint;
	// Optional filters (pointers to the caller's functions; null after construction, in the same
	// order of precedence) run on the node before its lattice points: anything but
	// NAV_SEARCH_INCLUDE is returned to the search, and clearing the flag skips the points of the
	// node. No function of these signatures exists in this build. Named by what they do
	const std::function< NavSearchResult_t( const CNavArea &, bool * ) > *m_pfnAreaFilter;
	const std::function< NavSearchResult_t( const CNavBlock &, bool * ) > *m_pfnBlockFilter;
	const std::function< NavSearchResult_t( const NavElemBase_t< 1 > &, bool * ) > *m_pfnElemFilter;
	// The node the search starts from (copied from a constructor argument); when it is empty the
	// search starts from the position
	NavElemBase_t< 1 > m_StartNode;
	// The node processor of the search (CNavSearchNodeProcessor_BFSLattice< NavAreaAdj_t >);
	// the destructor deletes it
	INavSearchNodeProcessor *m_pProcessor;
	// The start position of the search, copied from the constructor argument; when it is not
	// valid the processor's GetStartPos is used
	Vector m_vecStart;
};

#endif // SERVER_NAV_PATHFIND_H
