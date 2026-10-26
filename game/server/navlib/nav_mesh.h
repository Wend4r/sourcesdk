//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The navigation mesh of one spawn group.
//
//=============================================================================//

#ifndef NAV_MESH_H
#define NAV_MESH_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "mathlib/vector.h"
#include "mathlib/mathlib.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlpair.h"
#include "tier0/threadtools.h"
#include "tier1/utlhashmaplarge.h"
#include "nav_area.h"
#include "nav_ladder.h"

#include "schemacompiler/schemaon.h"

class CUtlBuffer;
class KeyValues3;
class CTraceFilter;
class INavEditor;
class INavObstructionManager;
class INavMarkupManager;
class INavDynamicConnectionsManager;
class INavEntityInterface;
class INavPhysicsInterface;
class INavMeshGameEventListener;
class CNavHullVDataFileManager;
class CNavGenInput;

//-----------------------------------------------------------------------------
// The spatial index of a mesh: one grid of square cells per hull, each cell listing the areas
// that overlap it. Cell (x, y) of a hull's grid is element x + y * m_nGridSizeX.
//-----------------------------------------------------------------------------
class CNavGrid
{
public:
	typedef CUtlVector< CNavArea * > Cell_t;

	// The cells of one hull.
	struct VecGridSplit_t : public CUtlLeanVector< Cell_t, int >
	{
	};

	int GetCellX( float x ) const
	{
		int nX = ( int )( ( x - m_flGridMinX ) / m_flGridCellSize );
		return nX < 0 ? 0 : ( nX < m_nGridSizeX - 1 ? nX : m_nGridSizeX - 1 );
	}

	int GetCellY( float y ) const
	{
		int nY = ( int )( ( y - m_flGridMinY ) / m_flGridCellSize );
		return nY < 0 ? 0 : ( nY < m_nGridSizeY - 1 ? nY : m_nGridSizeY - 1 );
	}

public:
	// Indexed by the areas' m_nHullIdx; sized to the mesh's hull count when it loads.
	CUtlLeanVector< VecGridSplit_t, int > m_HullGrids;
	// 300 units unless the mesh sets it.
	float m_flGridCellSize;
	int m_nGridSizeX;
	int m_nGridSizeY;
	// FLT_MAX while the grid has no extents.
	float m_flGridMinX;
	float m_flGridMinY;
};

//-----------------------------------------------------------------------------
// The areas selected in the nav editor. Splitting a selected area puts both halves in the set
// and takes the split area out.
//-----------------------------------------------------------------------------
class CNavSelectionSet
{
public:
	CUtlVector< CNavArea * > m_SetAreas;
};

//-----------------------------------------------------------------------------
// The names of the places (callouts) of a mesh.
//-----------------------------------------------------------------------------
class CNavPlaceDatabase
{
public:
	char **m_ppPlaceName;
	uint32 m_nPlaceCount;
};

//-----------------------------------------------------------------------------
// The places a nav file refers to, in file order; used while the file is read or written.
//-----------------------------------------------------------------------------
class CNavPlaceDirectory
{
public:
	CUtlVector< Place > m_Directory;
	bool m_bHasUnnamedAreas;
};

//-----------------------------------------------------------------------------
// Schema class: the agent of one nav hull, as scripts/nav_hulls.vdata defines it.
//-----------------------------------------------------------------------------
schema struct CNavHullVData
{
	TYPEMETA( MVDataRoot; MGetKV3ClassDefaults );
	DECLARE_SCHEMA_DATA_CLASS( CNavHullVData );

	bool m_bAgentEnabled; META( MPropertyFriendlyName = "Enabled"; MPropertyDescription = "Is this agent enabled for generation? ( will result in 0 nav areas for this agent if not )." );
	float m_agentRadius; META( MPropertyFriendlyName = "Radius"; MPropertyDescription = "Radius of navigating agent capsule." );
	float m_agentHeight; META( MPropertyFriendlyName = "Height"; MPropertyDescription = "Height of navigating agent capsule." );
	bool m_agentShortHeightEnabled; META( MPropertyFriendlyName = "Enable Crouch Height"; MPropertyDescription = "Enable shorter navigating agent capsules ( crouch ) in addition to regular height capsules." );
	float m_agentShortHeight; META( MPropertyFriendlyName = "Crouch height"; MPropertyDescription = "Crouch height of navigating agent capsules if enabled." );
	bool m_agentCrawlEnabled; META( MPropertyFriendlyName = "Enable Crawl Height"; MPropertyDescription = "Enable even shorter navigating agent capsules ( crawl ) in addition to regular height capsules." );
	float m_agentCrawlHeight; META( MPropertyFriendlyName = "Crawl height"; MPropertyDescription = "Crawl height of navigating agent capsules if enabled." );
	float m_agentMaxClimb; META( MPropertyFriendlyName = "Max Climb"; MPropertyDescription = "Max vertical offset that the agent simply ignores and walks over." );
	int m_agentMaxSlope; META( MPropertyFriendlyName = "Max Slope"; MPropertyDescription = "Max ground slope to be considered walkable." );
	float m_agentMaxJumpDownDist; META( MPropertyFriendlyName = "Max Jump Down Distance"; MPropertyDescription = "Max vertical offset at which to create a jump connection ( possibly one-way )." );
	float m_agentMaxJumpHorizDistBase; META( MPropertyFriendlyName = "Max Horizontal Jump Distance"; MPropertyDescription = "Max horizontal offset over which to create a jump connection ( actually a parameter into the true threshold function )." );
	float m_agentMaxJumpUpDist; META( MPropertyFriendlyName = "Max Jump Up Distance"; MPropertyDescription = "Max vertical offset at which to make a jump connection two-way." );
	int m_agentBorderErosion; META( MPropertyFriendlyName = "Border Erosion"; MPropertyDescription = "Border erosion in voxel units ( -1 to use default value based on agent radius )." );
	bool m_flowMapGenerationEnabled; META( MPropertyFriendlyName = "Hierarchical Nav"; MPropertyDescription = "Enables super node nav information to be generated" );
	float m_flowMapNodeMaxRadius; META( MPropertyFriendlyName = "Hierarchical Nav Max Super Node radius"; MPropertyDescription = "Maximum radius of a super node - larger means lower resolution" );
};

//-----------------------------------------------------------------------------
// The generation settings a mesh was built with, stored in its nav file. Meshes that ask for
// the project defaults warn when the stored settings differ from them.
//-----------------------------------------------------------------------------
class CNavGenParams
{
public:
	// One entry of HullParams: a hull's agent settings and its name. The settings are those of
	// CNavHullVData in another order, as the nav file reader and the NavAgent*_N gameinfo keys
	// store them.
	struct HullParams_t
	{
		bool m_bAgentEnabled;
		float m_flAgentRadius;
		int m_nAgentBorderErosion;
		float m_flAgentHeight;
		bool m_bAgentShortHeightEnabled;
		float m_flAgentShortHeight;
		bool m_bAgentCrawlEnabled;
		float m_flAgentCrawlHeight;
		float m_flAgentMaxClimb;
		int m_nAgentMaxSlope;
		float m_flAgentMaxJumpDownDist;
		float m_flAgentMaxJumpHorizDistBase;
		float m_flAgentMaxJumpUpDist;
		bool m_bFlowMapGenerationEnabled;
		float m_flFlowMapNodeMaxRadius;
		CUtlString m_sName;
	};

public:
	bool m_useProjectDefaults;
	float m_flTileSize;
	float m_flCellSize;
	float m_flCellHeight;
	int m_nRegionMinSize;
	int m_nRegionMergeSize;
	float m_flDetailSampleDist;
	float m_flDetailSampleMaxError;
	int m_nEdgeMaxLen;
	float m_flEdgeMaxError;
	int m_nVertsPerPoly;
	// NavSmallAreaOnEdgeRemovalSize of the project defaults. Read from settings version 7 on;
	// -1 by default.
	float m_flSmallAreaOnEdgeRemovalSize;

private:
	// Two strings read from settings version 12 on.
	CUtlString m_sUnk30;
	CUtlString m_sUnk38;

public:
	// One entry per hull; its count is the mesh's hull count.
	CUtlVector< HullParams_t > m_HullParams;

private:
	// Cleared in the project defaults.
	bool m_bUnk58;
	// Read from the nav file after the hull parameters (settings version 12 on); compared with
	// the project defaults.
	bool m_bUnk59;
};

//-----------------------------------------------------------------------------
// A markup volume (a box with a transform) given to generation and kept in the nav file.
//-----------------------------------------------------------------------------
struct NavGenMarkup_t
{
	Vector m_bbMin;
	Vector m_bbMax;
	matrix3x4_t m_transform;
	// 0xFFFF when the markup has no property.
	uint32 m_nPropertyIndex;

private:
	// 1 on construction, 0 once read from a nav file.
	bool m_bUnk4C;
};

//-----------------------------------------------------------------------------
// Integers of which the first N are kept inline and the rest in a vector. Movable meshes are
// identified by one, parsed from their ':'-separated mesh id string; the manager keys them by
// its hash.
//-----------------------------------------------------------------------------
template < typename T, int N >
class CUtlIntString
{
public:
	// Number of used entries in m_Inline.
	int m_nInlineCount;
	T m_Inline[ N ];
	// The entries after the first N.
	CUtlVector< T > m_Overflow;
};

//-----------------------------------------------------------------------------
// One area to add to a grid cell after a movable mesh update.
//-----------------------------------------------------------------------------
struct NavBatchUpdateAreaInGridOp_t
{
	CNavArea *m_pArea;
	// Cell index (y * width + x) in the grid.
	int m_nCellIndex;
	// 0 when queued by the geometry change of the area.
	bool m_bUnk0C;
};

//-----------------------------------------------------------------------------
// The areas associated with movable meshes, keyed by movable mesh hash.
//-----------------------------------------------------------------------------
class CNavMovableMeshCatalog
{
private:
	// The areas of each movable mesh whose m_nAttributeDynamicFlags lack 4.
	CUtlHashtable< uint32, CUtlVector< CNavArea * > * > m_AreasByMesh;
	// The areas of each movable mesh whose m_nAttributeDynamicFlags have 4.
	CUtlHashtable< uint32, CUtlHashtable< CNavArea * > * > m_DynamicAreasByMesh;
	// By area ID: the first and the second movable mesh the area was added for, -1 when unset.
	CUtlHashtable< uint32, CUtlPair< uint32, uint32 > > m_MeshesByArea;
};

//-----------------------------------------------------------------------------
// The areas of movable meshes (moving platforms) and their transforms. It has no virtuals.
//-----------------------------------------------------------------------------
class CNavMovableMeshesManager
{
public:
	struct MovableMeshContext_t
	{
		// The transform the areas were last moved with.
		matrix3x4_t m_AppliedTransform;
		// The last transform received; while the mesh is in m_SuspendedMeshes only this one is
		// updated.
		matrix3x4_t m_PendingTransform;
		// Bit 3: movable_mesh_gravity_follows_rotation.
		uint32 m_nFlags;
	};

	// Filled by several threads: a slot is taken by incrementing m_nCount and entries past the
	// preallocated m_Elements go to m_Overflow under m_OverflowMutex. The counter is on its own
	// cache line.
	template < typename T >
	struct AppendBuffer_t
	{
		CUtlLeanVector< T > m_Elements;
		CUtlLeanVector< T > m_Overflow;
		CAtomicMutex m_OverflowMutex;

	private:
		uint8 m_pad[ 128 - 32 - sizeof( CAtomicMutex ) ];

	public:
		int m_nCount;
		// -1 once the buffer is flushed.
		int m_nUnk84;

	private:
		uint8 m_pad88[ 120 ];
	};

private:
	// The mesh id of each movable mesh hash.
	CUtlHashMap< uint32, CUtlIntString< int, 6 > > m_MeshIds;
	CUtlHashMap< uint32, MovableMeshContext_t > m_MeshContexts;
	CNavMovableMeshCatalog m_Catalog;
	// Incremented by every transform update and stored in m_VertexUpdateSerials for each vertex
	// it moves, so a vertex shared by several areas moves once.
	int m_nUpdateSerial;
	// One per geometry vertex.
	CUtlVector< int > m_VertexUpdateSerials;
	// One per hull (CNavArea::m_nHullIdx): the grid cells to put moved areas in.
	CUtlVector< AppendBuffer_t< NavBatchUpdateAreaInGridOp_t > > m_GridUpdateOps;
	// One per geometry vertex: the position before the update.
	CUtlVector< Vector > m_VertexOldPositions;
	// The areas of each update.
	CUtlVector< CUtlVector< CNavArea * > > m_UpdateAreas;
	// One per hull (CNavArea::m_nHullIdx): the areas whose geometry changed.
	CUtlVector< AppendBuffer_t< CNavArea * > > m_ChangedAreas;
	// Movable mesh hashes whose areas are not moved by transform updates.
	CUtlHashtable< uint32 > m_SuspendedMeshes;
};

//-----------------------------------------------------------------------------
// The mesh the game's bots path on is the one of the active spawn group; every loaded spawn
// group with nav data has one.
//-----------------------------------------------------------------------------
abstract_class CNavMesh
{
public:
	virtual ~CNavMesh() = 0;

	// Called by LoadNavMesh with the area count before any area is created.
	virtual void PreLoadAreas( int nAreaCount ) = 0;
	virtual void UnloadAreas() = 0;
	// Constructs an area on nPolyIndex, in pExisting's memory when it is given, and adds it to
	// m_NavAreas.
	virtual CNavArea *CreateArea( int nPolyIndex, CNavArea *pExisting ) = 0;
	// Removes the area from m_NavAreas and deletes it; externally created areas are skipped.
	virtual void DestroyArea( CNavArea *pArea ) = 0;
	// Removes the ladder from m_Ladders and deletes it.
	virtual void DestroyLadder( CNavLadder *pLadder ) = 0;
	// Validates every area, then the grid against m_NavAreas.
	virtual void Validate() const = 0;
	// Destroys every ladder and area and empties the grid, the id hash and the movable meshes.
	virtual void ResetNavMesh() = 0;
	// Called when the mesh becomes the active one; raises the next area id above the mesh's own
	// ids and activates the managers.
	virtual void OnActivated() = 0;
	// Called on the active mesh when another one replaces it, and when it is destroyed while
	// active.
	virtual void OnDeactivated() = 0;
	virtual void Update() = 0;
	// Parses a nav file already read into memory; returns 0 on success. pszMapName is the name
	// the file was looked up by.
	virtual int LoadNavMesh( const char *pszMapName, const void *pData, uint32 nDataSize ) = 0;
	// Binds areas and ladders after loading and marks the mesh loaded; returns 0 on success.
	virtual int PostLoadNavMesh( uint32 nVersion ) = 0;
	// Called by the nav game system after its spawn group has loaded.
	virtual void PostSpawnGroupLoad() = 0;
	// The newest sub-version of the game's custom data; 0 for the base mesh.
	virtual uint32 GetSubVersionNumber() const = 0;
	// Custom data of derived meshes, after the areas in the nav file.
	virtual void SaveCustomData( CUtlBuffer &fileBuffer ) const = 0;
	virtual void LoadCustomData( CUtlBuffer &fileBuffer ) = 0;
	// Custom data of derived meshes, before the areas in the nav file.
	virtual void SaveCustomDataPreArea( CUtlBuffer &fileBuffer ) const = 0;
	virtual void LoadCustomDataPreArea( CUtlBuffer &fileBuffer ) = 0;
	// Replaces the mesh with generated geometry, taking the generation settings from input.
	// The first four arguments are the generated vertices, polygons and their data; their types
	// are not reconstructed.
	virtual void BuildFromNavGen( const void *pUnk1, const void *pUnk2, const void *pUnk3, const void *pUnk4, const CNavGenInput &input ) = 0;
	// Calls OnServerActivate on every area.
	virtual void OnServerActivate() = 0;
	// Called on round_start before every area's OnRoundRestart; resets the obstruction manager.
	virtual void OnRoundRestart() = 0;
	// Puts the area into the grid and the id hash; a movable mesh id other than -1 also
	// registers it with that movable mesh and sets NAV_AREA_MOVABLE. bNotifyObstructionManager
	// passes docks (NAV_AREA_DOCK) on to the obstruction manager.
	// The two overloads take their slots in this order on Linux and in the reverse order on
	// Windows, as MSVC groups overloads.
	virtual void AddNavArea( CNavArea *pArea, uint32 nMovableMeshId, bool bNotifyObstructionManager ) = 0;
	// Forwards to AddNavArea with no movable mesh.
	virtual void AddNavArea( CNavArea *pArea, bool bNotifyObstructionManager ) = 0;
	// Takes the areas out of the grid, the id hash, the movable meshes and the geometry, and
	// notifies the managers.
	virtual void RemoveNavArea( int nCount, CNavArea **ppAreas ) = 0;
	// Removes the areas that should be destroyed and has the others drop their connections to
	// them.
	virtual void RemoveInvalidNavAreas() = 0;
	// Sets the area's m_nMeshIndex.
	virtual void SetAreaMeshIndex( CNavArea *pArea, int nMeshIndex ) = 0;
	// The movable meshes' movable_mesh_gravity_follows_rotation settings. Save reads them from
	// 112-byte movable mesh records (the flag is the byte at 104), whose type is not
	// reconstructed; Load appends one flag per movable mesh to pGravityFollowsRotation.
	virtual void SaveMovableMeshSettings( KeyValues3 *pKV, const void *pUnk ) const = 0;
	virtual void LoadMovableMeshSettings( const KeyValues3 *pKV, CUtlVector< bool > *pGravityFollowsRotation ) = 0;
	// Empty in the game's meshes.
	virtual void unk029( void *p ) = 0;
	// Empty in the game's meshes; LoadNavMesh passes the KeyValues3 block that follows the pre-area
	// custom data of a version 36 nav file.
	virtual void LoadCustomDataKV3( const KeyValues3 *pKV ) = 0;
	// Forwards to SaveNavGenHullParams.
	virtual void SaveHullParams( KeyValues3 *pKV ) const = 0;
	// Forwards to LoadNavGenHullParams; LoadNavMesh calls it with the nav file's KeyValues3 block.
	virtual void LoadHullParams( const KeyValues3 *pKV ) = 0;
	// Empty in the game's meshes; called by BuildFromNavGen once the areas are added.
	virtual void ReadNavGenMarkup( const CNavGenInput &input ) = 0;
	// NavGenParams.HullParams: the Name, FlowMap_Enabled and FlowMap_NodeMaxRadius of every
	// hull in m_NavGenParams.
	virtual void LoadNavGenHullParams( const KeyValues3 *pKV ) = 0;
	virtual void SaveNavGenHullParams( KeyValues3 *pKV ) const = 0;

	int GetNavAreaCount() const { return m_NavAreas.Count(); }
	CNavArea *GetNavArea( int i ) const { return m_NavAreas[ i ]; }

	int GetLadderCount() const { return m_Ladders.Count(); }
	CNavLadder *GetLadder( int i ) const { return m_Ladders[ i ]; }

	int GetHullCount() const { return m_NavGenParams.m_HullParams.Count(); }

public:
	// Owned by the mesh; an area's m_nMeshIndex is its index here.
	CUtlVector< CNavArea * > m_NavAreas;
	// Owned.
	INavEditor *m_pEditor;
	INavObstructionManager *m_pObstructionManager;

private:
	// An interface told of activation and of added and removed areas; the game's own mesh
	// construction does not set it.
	void *m_pAreaListener;

public:
	INavMarkupManager *m_pMarkupManager;
	INavDynamicConnectionsManager *m_pDynamicConnectionsManager;
	// Owned.
	INavEntityInterface *m_pEntityInterface;
	// Owned.
	INavPhysicsInterface *m_pPhysicsInterface;
	// Owned; created by the first LoadNavMesh.
	CNavPlaceDirectory *m_pPlaceDirectory;
	// Owned; restarts the mesh on round_start.
	INavMeshGameEventListener *m_pNavMeshGameEventListener;

private:
	// An interface ticked by Update and told of movable mesh changes.
	void *m_pMovableMeshListener;
	// -1 while Update runs, then the value of a serial shared by every mesh, which each Update
	// advances (wrapping from 0x7FFF to 1).
	int16 m_nLastUpdateSerial;

public:
	CUtlString m_NavFilename;
	CNavSelectionSet m_SelectionSet;
	CNavPlaceDatabase m_PlaceDatabase;
	// Owned; released through m_pPhysicsInterface.
	CTraceFilter *m_pTraceFilterWalkableEntities;
	CTraceFilter *m_pTraceFilterGroundEntities;
	CNavGrid m_Grid;
	// Area id to area.
	CUtlHashtable< uint32, CNavArea * > m_hashTable;
	// Set by PostLoadNavMesh, cleared by ResetNavMesh.
	bool m_bIsLoaded;
	// Read from the nav file; LoadNavMesh warns that the mesh needs a full nav_analyze when it
	// is not set.
	bool m_bIsAnalyzed;

private:
	// Cleared by ResetNavMesh.
	bool m_bUnk102;

public:
	uint32 m_nFileSubVersion;
	CUtlVector< CNavLadder * > m_Ladders;
	CUtlVector< NavGenMarkup_t > m_NavMarkups;
	CNavGenParams m_NavGenParams;
	CNavGeometryManager m_GeometryManager;
	CNavMovableMeshesManager m_MovableMeshesManager;

private:
	// The next four members are one object whose methods take its address; no class name
	// identifies it.
	// Storage of destroyed areas: CreateArea reuses the last one for the next area it builds
	// on a polygon, and the destructor frees what is left.
	CUtlVector< CNavArea * > m_RecycledAreas;
	// Areas queued for removal. Flushing marks them with dynamic attribute bit 3, removes them
	// from the mesh, destroys them and moves their storage to m_RecycledAreas.
	CUtlVector< CNavArea * > m_PendingAreaRemovals;
	// A vector only emptied and freed by the object's destructor; nothing adds to it.
	uint8 m_Unk400[ 0x18 ];
	// The mesh the object belongs to; the active mesh is used while it is null.
	CNavMesh *m_pRecyclerMesh;

public:
	// Loaded from scripts/nav_hulls.vdata; null when that file is missing.
	CNavHullVDataFileManager *m_pHullVDataFileManager;
};

#include "schemacompiler/schemaoff.h"

#endif // NAV_MESH_H
