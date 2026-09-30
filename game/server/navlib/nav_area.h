//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: A walkable polygon of the navigation mesh and its connections.
//
//=============================================================================//

#ifndef NAV_AREA_H
#define NAV_AREA_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "tier1/utlvector.h"

class CNavArea;
class CNavMesh;
class CNavLadder;
class CUtlBuffer;
struct LegacyNavData_t;

// The most corners, and so the most connection directions, an area has.
#define NAV_MAX_POLY_VERTICES 4

// Teams with their own per-area state (player counts, danger, blocking).
#define NAV_MAX_TEAMS 2

// A place (callout) id; 0 is no place.
typedef uint32 Place;

//-----------------------------------------------------------------------------
// Static attributes of an area, the bits of CNavAttribute. Bits from
// NAV_ATTR_FIRST_GAME_INDEX up to NAV_ATTR_LAST_INDEX are defined by the game; those two are
// bit indices, not masks.
//-----------------------------------------------------------------------------
enum NavAttributeEnum : uint64
{
	NAV_MESH_NONE = 0x0,
	NAV_MESH_JUMP = 0x2,
	NAV_MESH_NO_JUMP = 0x8,
	NAV_MESH_STOP = 0x10,
	NAV_MESH_RUN = 0x20,
	NAV_MESH_WALK = 0x40,
	NAV_MESH_AVOID = 0x80,
	NAV_MESH_TRANSIENT = 0x100,
	NAV_MESH_DONT_HIDE = 0x200,
	NAV_MESH_STAND = 0x400,
	NAV_MESH_NO_HOSTAGES = 0x800,
	NAV_MESH_STAIRS = 0x1000,
	NAV_MESH_NO_MERGE = 0x2000,
	NAV_MESH_OBSTACLE_TOP = 0x4000,
	NAV_MESH_NON_ZUP = 0x8000,
	NAV_MESH_CROUCH_HEIGHT = 0x10000,
	NAV_MESH_NON_ZUP_TRANSITION = 0x20000,
	NAV_MESH_CRAWL_HEIGHT = 0x40000,

	NAV_ATTR_FIRST_GAME_INDEX = 0x13,
	NAV_ATTR_LAST_INDEX = 0x3f,
};

//-----------------------------------------------------------------------------
// Runtime state bits of an area, kept apart from its attributes.
//-----------------------------------------------------------------------------
enum NavAttributeDynamicType : uint32
{
	NAV_AREA_NONE = 0x0,
	NAV_AREA_UNDER_WATER = 0x1,
	NAV_AREA_UNDER_WATER_DEEP = 0x2,
	NAV_AREA_EXTERNALLY_CREATED = 0x4,
	NAV_AREA_SHOULD_BE_DESTROYED = 0x8,
	NAV_AREA_CREATED_BY_OBSTACLE_MGR = 0x10,
	NAV_AREA_SPLIT_BY_OBSTACLE_MGR = 0x20,
	NAV_AREA_SPLIT_OBS_CONTAINED = 0x40,
	NAV_AREA_SPLIT_OBS_BASE_CONTAINED = 0x80,
	NAV_AREA_HAS_LADDERS = 0x100,
	NAV_AREA_NAV_LINK = 0x200,
	NAV_AREA_NAV_LINK_TERMINUS = 0x400,
	NAV_AREA_CONNECTED_TO_NAV_LINK_OUT = 0x800,
	NAV_AREA_CONNECTED_TO_NAV_LINK_IN = 0x1000,
	NAV_AREA_MOVABLE = 0x2000,
	NAV_AREA_DOCK = 0x4000,
	NAV_AREA_DOCKING_CANDIDATE = 0x8000,
	NAV_AREA_BOUNDARY = 0x10000,
	NAV_AREA_HAS_TACTICAL_SEARCH_ANNOTATIONS = 0x20000,
	NAV_AREA_DEFORMABLE = 0x40000,
	NAV_AREA_DEFORMABLE_DOCK = 0x80000,
	NAV_AREA_LINK_AUTO_ADJUST = 0x100000,
};

// The sides of a quad area; the sides of a polygon area are its edges instead.
enum NavDirType
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,

	NUM_NAV_DIR_TYPE_DIRECTIONS = 4,
};

// How a path segment is entered. Values 0 to 3 cross the edge with that index; the game uses
// further values (7, 8) for other floor transitions.
enum NavTraverseType
{
	GO_NORTH = 0,
	GO_EAST = 1,
	GO_SOUTH = 2,
	GO_WEST = 3,
	GO_SAME_AREA = 4,
	GO_LADDER_UP = 5,
	GO_LADDER_DOWN = 6,
};

// What an area or position visitor returns to the mesh's iteration and search helpers.
enum NavSearchResult_t
{
	// Stops the iteration.
	NAV_SEARCH_HALT = 0,
	// Keeps going; a search also expands from the visited area.
	NAV_SEARCH_INCLUDE = 1,
	// Keeps going without expanding from the visited area.
	NAV_SEARCH_EXCLUDE = 2,
};

//-----------------------------------------------------------------------------
// Selects the navigation hull (agent size) a mesh was built for; -1 matches every hull.
//-----------------------------------------------------------------------------
struct NavHull_t
{
	int m_nHullIdx;
};

class CNavFlags
{
public:
	uint64 m_Flags;
};

// A set of NavAttributeEnum bits.
class CNavAttribute : public CNavFlags
{
public:
	bool HasAny( uint64 nMask ) const { return ( m_Flags & nMask ) != 0; }
};

//-----------------------------------------------------------------------------
// One polygon of the geometry manager: indices into its vertex array.
//-----------------------------------------------------------------------------
struct NavPolyDef_t
{
	int m_nVertices[ NAV_MAX_POLY_VERTICES ];
	int8 m_nVertexCount;
};

//-----------------------------------------------------------------------------
// The shared vertices and polygons the areas of a mesh are made of. Every mesh embeds one.
//-----------------------------------------------------------------------------
class CNavGeometryManager
{
private:
	uint8 m_Unk00[ 0x20 ];

public:
	CUtlVector< Vector > m_Vertices;

private:
	uint8 m_Unk38[ 0x30 ];

public:
	CUtlVector< NavPolyDef_t > m_Polys;

private:
	uint8 m_Unk80[ 0x18 ];
};

//-----------------------------------------------------------------------------
// A connection to a neighbouring area. m_nBackEdge is the edge on the neighbour's side that
// leads back; the area id is only valid while the mesh is loading.
//-----------------------------------------------------------------------------
struct CNavConnect
{
	union
	{
		uint32 m_nAreaId;
		CNavArea *m_pArea;
	};
	int8 m_nBackEdge;
};

// Kept for code written against the old name.
typedef CNavConnect NavConnect_t;

//-----------------------------------------------------------------------------
// The connections through one side of an area: a count followed by that many entries, in one
// allocation. Unlike the generic ultra-conservative vector, an empty one holds a null pointer.
//-----------------------------------------------------------------------------
class CNavConnectVector
{
public:
	struct Data_t
	{
		int m_Size;
		CNavConnect m_Elements[ 1 ];

		int Count() const { return m_Size; }
		const CNavConnect &Element( int i ) const { return m_Elements[ i ]; }
	};

	int Count() const { return m_pData ? m_pData->m_Size : 0; }
	const CNavConnect &Element( int i ) const { return m_pData->m_Elements[ i ]; }
	const CNavConnect &operator[]( int i ) const { return m_pData->m_Elements[ i ]; }

public:
	Data_t *m_pData;
};

typedef CNavConnectVector::Data_t NavConnectList_t;

//-----------------------------------------------------------------------------
// The geometry of an area: one polygon of the mesh's geometry manager, its centre, normal,
// bounds and attributes.
//-----------------------------------------------------------------------------
abstract_class CNavPoly
{
public:
	// Recomputes the geometry unless nPolyIndex is -1.
	virtual void SetPolyIndex( int nPolyIndex ) = 0;
	// Centre, normal, bounds and flatness from the polygon's vertices.
	virtual void GeometryChanged() = 0;
	// ORs the other polygon's attributes, and its NAV_AREA_MOVABLE and NAV_AREA_DOCK bits, into
	// this one's.
	virtual void InheritAttributes( const CNavPoly *pOther ) = 0;

	// Corners of the polygon, 0 when it has none.
	int GetVertexCount() const
	{
		return m_nPolyIndex == -1 ? 0 : m_pGeometry->m_Polys[ m_nPolyIndex ].m_nVertexCount;
	}

	const Vector &GetVertex( int i ) const
	{
		return m_pGeometry->m_Vertices[ m_pGeometry->m_Polys[ m_nPolyIndex ].m_nVertices[ i ] ];
	}

	// Side i runs from vertex i to vertex i + 1; a polygon-less area still has four sides.
	int GetDirectionCount() const
	{
		return m_nPolyIndex == -1 ? NAV_MAX_POLY_VERTICES : m_pGeometry->m_Polys[ m_nPolyIndex ].m_nVertexCount;
	}

	const Vector &GetCenter() const { return m_vecCenter; }
	const Vector &GetNormal() const { return m_vecNormal; }
	const Vector &GetExtentMins() const { return m_vecExtentMins; }
	const Vector &GetExtentMaxs() const { return m_vecExtentMaxs; }
	bool IsFlat() const { return ( m_nPolyFlags & 1 ) != 0; }

	const CNavAttribute &GetAttributes() const { return m_AreaAttributes; }
	bool HasAttributes( uint64 nMask ) const { return m_AreaAttributes.HasAny( nMask ); }
	uint32 GetDynamicAttributes() const { return m_nAttributeDynamicFlags; }
	bool HasDynamicAttributes( uint32 nMask ) const { return ( m_nAttributeDynamicFlags & nMask ) != 0; }

public:
	int m_nPolyIndex;
	Vector m_vecCenter;
	// (0, 0, 1) until the geometry is computed.
	Vector m_vecNormal;
	Vector m_vecExtentMins;
	Vector m_vecExtentMaxs;
	CNavAttribute m_AreaAttributes;
	// NavAttributeDynamicType bits.
	uint32 m_nAttributeDynamicFlags;
	// The geometry manager of the mesh the area was created for.
	CNavGeometryManager *m_pGeometry;
	// The hull (NavHull_t::m_nHullIdx) the area was built for.
	uint8 m_nHullIdx;
	// Set while the area is listed in its mesh's grid.
	bool m_bIsInGrid;
	// Bit 0: the polygon's corners are within 0.05 units of one height.
	uint8 m_nPolyFlags;
};

//-----------------------------------------------------------------------------
// An area of the navigation mesh. Its own members start in the tail padding of CNavPoly under
// the Itanium ABI, which is why the first ones move by four bytes and the rest do not.
//
// Pathfinding state (parent area, how it was reached) is not stored here; the game keeps it in
// a per-thread table indexed by m_nMeshIndex.
//-----------------------------------------------------------------------------
abstract_class CNavArea : public CNavPoly
{
public:
	virtual ~CNavArea() = 0;

	// Maps m_nMeshIndex through vecRemap when it is in range.
	virtual void RemapIndices( const CUtlVector< int > &vecRemap ) = 0;
	// Consistency checks, compiled out of release builds; with bCheckConnections the release
	// build still walks every connection.
	virtual void Validate( bool bCheckConnections ) = 0;
	virtual void SetPlace( Place place ) = 0;
	virtual Place GetPlace() const = 0;
	// Writes the id, the saved attribute bits, the connections and the rest of the area to the
	// nav file. The first argument is not used by the base.
	virtual void Save( const void *pUnk, CUtlBuffer &fileBuffer ) const = 0;
	// Reads what Save wrote; pLegacyData collects data of old file versions for the mesh to
	// convert. The last argument is always null in the mesh's loader.
	virtual void Load( CNavMesh *pMesh, CUtlBuffer &fileBuffer, uint32 nVersion, uint32 nSubVersion, LegacyNavData_t *pLegacyData, void *pUnk ) = 0;
	// Resolves connection and ladder ids into pointers; returns non-zero on corrupt data.
	virtual int PostLoad( CNavMesh *pMesh ) = 0;
	virtual void OnServerActivate() = 0;
	// Called for every area on round_start.
	virtual void OnRoundRestart() = 0;
	virtual void OnRoundRestartPreEntity() = 0;
	// Drops connections to areas that are gone or no longer in a mesh.
	virtual void RemoveInvalidNavAreas() = 0;
	// Game analysis step; the Counter-Strike area computes its approach areas here. The
	// argument is not read by the game's areas.
	virtual void CustomAnalysis( bool bIncremental ) = 0;

	uint32 GetID() const { return m_nID; }
	CNavMesh *GetMesh() const { return m_pMesh; }

	// The areas reached through side nDir; null when there are none.
	const NavConnectList_t *GetConnections( int nDir ) const { return m_connect[ nDir ].m_pData; }
	// One-way connections into this area through side nDir.
	const NavConnectList_t *GetIncomingConnections( int nDir ) const { return m_incomingConnect[ nDir ].m_pData; }

	// A team of -1 asks whether the area is blocked for anyone.
	bool IsBlocked( int nTeamID ) const
	{
		if ( nTeamID == -1 )
			return m_nBlockedFlags != 0;

		return ( ( m_nBlockedFlags >> ( nTeamID & 1 ) ) & 1 ) != 0;
	}

public:
	// Unique within the process; handed out in creation order.
	uint32 m_nID;
	// The area's index in its mesh's m_NavAreas.
	int m_nMeshIndex;
	// Bumped by every geometry update.
	uint32 m_nGeometryRevision;
	// One bit per team; bit 0 for even team numbers, bit 1 for odd ones.
	uint8 m_nBlockedFlags;
	// Height in units of an avoidance obstacle standing on the area, clamped to 255.
	uint8 m_nAvoidanceObstacleHeight;

	CNavConnectVector m_connect[ NAV_MAX_POLY_VERTICES ];
	CNavConnectVector m_incomingConnect[ NAV_MAX_POLY_VERTICES ];

	CNavMesh *m_pMesh;
};

//-----------------------------------------------------------------------------
// A spot to hide in or watch from. The flags come straight from the nav file.
//-----------------------------------------------------------------------------
class HidingSpot
{
public:
	enum
	{
		IN_COVER = 0x01,
		GOOD_SNIPER_SPOT = 0x02,
		IDEAL_SNIPER_SPOT = 0x04,
		EXPOSED = 0x08,
	};

	const Vector &GetPosition() const { return m_pos; }
	uint32 GetID() const { return m_id; }
	const CNavArea *GetArea() const { return m_area; }
	int GetFlags() const { return m_flags; }
	bool HasGoodCover() const { return ( m_flags & IN_COVER ) != 0; }
	bool IsGoodSniperSpot() const { return ( m_flags & GOOD_SNIPER_SPOT ) != 0; }
	bool IsIdealSniperSpot() const { return ( m_flags & IDEAL_SNIPER_SPOT ) != 0; }
	bool IsExposed() const { return ( m_flags & EXPOSED ) != 0; }

public:
	Vector m_pos;
	uint32 m_id;
	uint32 m_marker;
	CNavArea *m_area;
	uint8 m_flags;
	// Spots created by entities are not saved.
	bool m_bIsSaved;
};

// A spot to look at while crossing an area, t along the encounter's path.
struct SpotOrder
{
	float t;
	union
	{
		HidingSpot *spot;
		uint32 id;
	};
};

//-----------------------------------------------------------------------------
// One way through an area, from one neighbour to another, and the spots to watch on the way.
//-----------------------------------------------------------------------------
struct SpotEncounter
{
	CNavConnect from;
	int fromDir;
	CNavConnect to;
	int toDir;

private:
	uint8 m_Unk2C[ 0x1C ];

public:
	CUtlVector< SpotOrder > spots;
};

#endif // NAV_AREA_H
