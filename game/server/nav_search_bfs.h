//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The breadth-first searches of the navigation mesh: the search itself, the node
//          processor that drives it and the lattice it searches.
//
//=============================================================================//

#ifndef NAV_SEARCH_BFS_H
#define NAV_SEARCH_BFS_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "mathlib/vector.h"
#include "navlib/nav_area.h"

class CNavArea;
class CNavBlock;
class CNavPathCost;
class CNavSearchWrapper;
struct NavAreaAdj_t;
// The information handed to a search callback with the visited node; it starts with the cost of
// the node. Its declaration is not reconstructed
struct NavSearchInfo_t;
// The information handed to an include filter with the candidate node (the node it is reached
// from and the step). Its declaration is not reconstructed
struct NavIncludeInfo_t;
// What an include filter returns: 0 drops the candidate for the rest of the search (it is marked
// visited), 1 skips it this time, anything else adds it to the open list. The names of the
// values are not reconstructed
enum NavIncludeResult_t : int;

//-----------------------------------------------------------------------------
// A node of a search: an area or a block of the fly space. 24 bytes. Named by what it holds
//-----------------------------------------------------------------------------
template < int N >
struct NavElemBase_t
{
	// 1 when the node is an area
	int m_nType;
	CNavArea *m_pArea;
	CNavBlock *m_pBlock;
};

//-----------------------------------------------------------------------------
// The breadth-first search over the nav mesh: an open list ordered by cost (shared per thread),
// the visit marks, the limits, the callbacks run on every visited node and the filters that
// decide which neighbours are added. 328 bytes on Linux, 520 on Windows (the std::function
// members differ in size). Not polymorphic; the node processor embeds one, the bot behaviours
// build one on the stack.
//-----------------------------------------------------------------------------
class CNavBreadthFirstSearch
{
public:
	// The pooled buffer of visit marks a search holds while it runs: a 2 MB array cleared each time
	// the 16-bit generation wraps. 16 bytes. Named by what it holds
	struct VisitMarks_t
	{
		uint16 *m_pMarks;
		uint16 m_nGeneration;
		bool m_bInUse;
	};

public:
	// A 104-byte object: the start position at +4, an optional bounding sphere (the flag at +32,
	// the centre at +36, the squared minimum and maximum distance at +48 / +52) the visited nodes
	// are tested against, a debug recorder at +88. Its type is not reconstructed
	void *m_pSearchBounds;
	// Which neighbours a node expands to (handed to the neighbour collection); 1 after
	// construction. Named by what it holds
	uint32 m_nAdjacencyFlags;
	// Handed to the lattice point generation of the node processor. Named by what it holds
	uint32 m_nLatticeFlags;
	// The callbacks run on the visited node, the first one set wins in the order elem, area,
	// block: NAV_SEARCH_HALT ends the search, NAV_SEARCH_EXCLUDE does not expand the node
	std::function< NavSearchResult_t( CNavArea &, const NavSearchInfo_t * ) > m_fnVisitArea;
	std::function< NavSearchResult_t( CNavBlock &, const NavSearchInfo_t * ) > m_fnVisitBlock;
	std::function< NavSearchResult_t( const NavElemBase_t< 1 > &, const NavSearchInfo_t * ) > m_fnVisitElem;
	// The filters run on every neighbour before it is added (the elem filter wins, then the area
	// or the block filter of the node's kind)
	std::function< NavIncludeResult_t( const CNavArea &, const NavIncludeInfo_t & ) > m_fnIncludeArea;
	std::function< NavIncludeResult_t( const CNavBlock &, const NavIncludeInfo_t & ) > m_fnIncludeBlock;
	std::function< NavIncludeResult_t( const NavElemBase_t< 1 > &, const NavIncludeInfo_t & ) > m_fnIncludeElem;
	// Collects the visited nodes and replays them in its own order once the open list is empty
	// (32-byte node records, a std::function at +56 run before the replay). Its type is not
	// reconstructed. Named by what it does
	void *m_pResultCollector;
	// The cost of the steps: the one in use (given by the caller or m_pOwnedPathCost) and the cost
	// of the hull the search was created for, which the search deletes
	const CNavPathCost *m_pPathCost;
	CNavPathCost *m_pOwnedPathCost;
	// Neighbours farther than this along the path (plus 400) are not added; -1 for no limit
	float m_flMaxPathDistance;
	// Neighbours farther than this from m_vecStartPos (or, without a start position, costlier than
	// it plus 400) are not added; -1 for no limit. The start of a search sets it to the remaining
	// range of the lattice
	float m_flMaxRadius;
	VisitMarks_t *m_pVisitMarks;
	// The name of the search ("FindConnectedBoundaryEdgesWithinCircle")
	const char *m_pszName;
	// The debug recorder the rejected (impassable) neighbours are recorded into (records appended to
	// its list at +288); its type is not reconstructed
	void *m_pDebugRecorder;
	// The id the rejected neighbours are recorded with
	int m_nDebugRecordId;
	// Set by the node processor next to m_nDebugRecordId; no reader was found
	int m_nDebugRecordId2;
	int m_nDebugRecordId3;
	// m_pSearchBounds was created by the search (and is deleted with it)
	bool m_bOwnsSearchBounds;
	// The node being visited, its cost and its distance along the path
	NavElemBase_t< 1 > m_CurrentNode;
	float m_flCurrentCost;
	float m_flCurrentDistance;
	// The start position of the search; NaN when the search started from nodes without one
	Vector m_vecStartPos;
	// The position in m_pResultCollector while its nodes are replayed; -1 before the replay
	int m_nResultIndex;
};

//-----------------------------------------------------------------------------
// Processes the nodes of a search (CNavSearchNodeProcessor_BFSLattice< NavAreaAdj_t >).
//-----------------------------------------------------------------------------
abstract_class INavSearchNodeProcessor
{
public:
	virtual ~INavSearchNodeProcessor() {}
	// Start the search: clear the current node and seed the open list from the nearest area to a
	// position (false without a nav mesh or a nearby area), from one node (with the start position,
	// or the node's centre when pvecPos is null) or from a list of areas, then step to the first
	// node. With a range set, the remaining range becomes m_flMaxRadius. One overloaded name: the
	// Windows table has them in reverse order. Named by what they do
	virtual bool StartSearch( const Vector &vecPos ) = 0;
	virtual bool StartSearch( const NavElemBase_t< 1 > &node, const Vector *pvecPos ) = 0;
	virtual bool StartSearch( const CUtlVector< CNavArea * > &areas ) = 0;
	// Whether the current node is an area or a block. Named by what it does
	virtual bool HasCurrentNode() const = 0;
	// The current node and its cost. Named by what they return
	virtual const NavElemBase_t< 1 > *GetCurrentNode() = 0;
	virtual float GetCurrentCost() const = 0;
	// Fills the CUtlVector at pPoints with the lattice points around the current node (20-byte
	// points starting with the position; their declaration is not reconstructed). Named by what it
	// does
	virtual bool GetLatticePoints( void *pPoints ) = 0;
	// Steps to the next node: nMode 0 empties the open list and clears the current node, 2 clears
	// the current node before the step. Named by what it does
	virtual bool Step( int nMode ) = 0;
	// Empties the open list and clears the current node. Named by what it does
	virtual void ResetSearch() = 0;
	// The radius of the outermost lattice ring. Named by what it returns
	virtual float GetMaxRange() const = 0;
	// The start position of m_pSearchBounds. Named by what it returns
	virtual Vector GetStartPos() const = 0;
	// Set CNavBreadthFirstSearch::m_flMaxPathDistance / m_flMaxRadius. Named by what they set
	virtual void SetMaxPathDistance( float flDistance ) = 0;
	virtual void SetMaxRadius( float flRadius ) = 0;
	// Empty
	virtual void Stub_15() = 0;
	// Set or clear bits of m_nLatticeFlags / m_nAdjacencyFlags, returning the old flags. Named by
	// what they set
	virtual uint32 SetLatticeFlags( uint32 nBits, bool bSet ) = 0;
	virtual uint32 SetAdjacencyFlags( uint32 nBits, bool bSet ) = 0;
	// Point m_fnIncludeArea or m_fnIncludeBlock at the caller's function (kept by reference); an
	// elem function becomes the area filter. One overloaded name: the Windows table has them in
	// reverse order. Named by what they set
	virtual void SetIncludeFilter( std::function< NavIncludeResult_t( const CNavArea &, const NavIncludeInfo_t & ) > &fnFilter ) = 0;
	virtual void SetIncludeFilter( std::function< NavIncludeResult_t( const CNavBlock &, const NavIncludeInfo_t & ) > &fnFilter ) = 0;
	virtual void SetIncludeFilter( std::function< NavIncludeResult_t( const NavElemBase_t< 1 > &, const NavIncludeInfo_t & ) > &fnFilter ) = 0;
	// Set m_pResultCollector, m_pszName, m_pDebugRecorder and the three debug record ids. Named by
	// what they set
	virtual void SetResultCollector( void *pCollector ) = 0;
	virtual void SetSearchName( const char *pszName ) = 0;
	virtual void SetDebugRecorder( void *pRecorder ) = 0;
	virtual void SetDebugRecordId( int nId ) = 0;
	virtual void SetDebugRecordId2( int nId ) = 0;
	virtual void SetDebugRecordId3( int nId ) = 0;
	// Returns m_pSearchBounds. Named by what it returns
	virtual void *GetSearchBounds() const = 0;
	// Returns false
	virtual bool Stub_28() const = 0;
};

//-----------------------------------------------------------------------------
// The node processor of the breadth-first lattice search: a breadth-first search over the nav
// mesh and the rings of lattice points generated around each visited node. 384 bytes on Linux,
// 576 on Windows. The game uses CNavSearchNodeProcessor_BFSLattice< NavAreaAdj_t >.
//-----------------------------------------------------------------------------
template < class T >
abstract_class CNavSearchNodeProcessor_BFSLattice : public INavSearchNodeProcessor
{
public:
	// One ring of lattice points: their spacing and the outer radius of the ring. Named by what it
	// holds
	struct LatticeRing_t
	{
		float m_flSpacing;
		float m_flRadius;
	};

	// The rings of the lattice, innermost first (at most three). 28 bytes. Named by what it holds
	struct LatticeRings_t
	{
		int m_nRings;
		LatticeRing_t m_Rings[ 3 ];
	};

	// Generates the lattice points around a node from the rings it points at. Named by what it
	// holds
	struct LatticeGenerator_t
	{
		const LatticeRings_t *m_pRings;
	};

public:
	CNavBreadthFirstSearch m_Search;
	// The two point generators over m_Rings: around any node, or only for areas inside the
	// bounding sphere of the search bounds (the generator m_bBoundedLattice selects)
	LatticeGenerator_t m_Lattice;
	LatticeGenerator_t m_BoundedLattice;
	LatticeRings_t m_Rings;
	bool m_bBoundedLattice;
};

//-----------------------------------------------------------------------------
// A breadth-first search over the nav mesh (the game uses CNavBFSLattice< NavAreaAdj_t > and
// CNavBFSLatticeFull_Area, which adds no data). 24 bytes. The constructor creates the node
// processor and the search wrapper that owns it.
//-----------------------------------------------------------------------------
template < class T >
abstract_class CNavBFSLattice
{
public:
	virtual ~CNavBFSLattice() = 0;

public:
	// The destructor deletes it
	CNavSearchWrapper *m_pSearch;
	// The node processor (CNavSearchNodeProcessor_BFSLattice< T >); m_pSearch owns it
	INavSearchNodeProcessor *m_pProcessor;
};

#endif // NAV_SEARCH_BFS_H
