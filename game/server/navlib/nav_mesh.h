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
#include "tier1/utlvector.h"
#include "tier1/utlleanvector.h"
#include "tier1/utlhashtable.h"
#include "nav_area.h"
#include "nav_ladder.h"

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
struct CNavHullVData
{
	bool m_bAgentEnabled;
	float m_agentRadius;
	float m_agentHeight;
	bool m_agentShortHeightEnabled;
	float m_agentShortHeight;
	bool m_agentCrawlEnabled;
	float m_agentCrawlHeight;
	float m_agentMaxClimb;
	int m_agentMaxSlope;
	float m_agentMaxJumpDownDist;
	float m_agentMaxJumpHorizDistBase;
	float m_agentMaxJumpUpDist;
	float m_agentBorderErosion;
	bool m_flowMapGenerationEnabled;
	float m_flowMapNodeMaxRadius;
};

//-----------------------------------------------------------------------------
// The generation settings a mesh was built with, stored in its nav file. Meshes that ask for
// the project defaults warn when the stored settings differ from them.
//-----------------------------------------------------------------------------
class CNavGenParams
{
public:
	// One entry of HullParams: a hull's agent settings and its name.
	struct HullParams_t
	{
		CNavHullVData m_Hull;
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

private:
	// Read from settings version 7 on; -1 by default.
	float m_flUnk2C;
	// Two strings read from settings version 12 on.
	CUtlString m_sUnk30;
	CUtlString m_sUnk38;

public:
	// One entry per hull; its count is the mesh's hull count.
	CUtlVector< HullParams_t > m_HullParams;

private:
	uint8 m_Unk58[ 2 ];
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
// The areas of movable meshes (moving platforms) and their transforms. It has no virtuals; its
// members are not reconstructed.
//-----------------------------------------------------------------------------
class CNavMovableMeshesManager
{
private:
	uint8 m_Data[ 0x1A0 ];
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
	virtual void Unk_OnActivated() = 0;
	// Called on the active mesh when another one replaces it, and when it is destroyed while
	// active.
	virtual void Unk_OnDeactivated() = 0;
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
	// Forwards to AddNavArea with no movable mesh.
	virtual void Unk_AddNavArea( CNavArea *pArea, bool bUnk ) = 0;
	// Puts the area into the grid and the id hash; a movable mesh id other than -1 also
	// registers it with that movable mesh and sets NAV_AREA_MOVABLE. bUnk passes docks
	// (NAV_AREA_DOCK) on to the obstruction manager.
	virtual void AddNavArea( CNavArea *pArea, uint32 nMovableMeshId, bool bUnk ) = 0;
	// Takes the areas out of the grid, the id hash, the movable meshes and the geometry, and
	// notifies the managers.
	virtual void RemoveNavArea( int nCount, CNavArea **ppAreas ) = 0;
	// Removes the areas that should be destroyed and has the others drop their connections to
	// them.
	virtual void RemoveInvalidNavAreas() = 0;
	// Sets the area's m_nMeshIndex.
	virtual void Unk_SetAreaMeshIndex( CNavArea *pArea, int nMeshIndex ) = 0;
	// The movable meshes' movable_mesh_gravity_follows_rotation settings.
	virtual void Unk_SaveMovableMeshSettings( KeyValues3 *pKV, const void *pUnk ) const = 0;
	virtual void Unk_LoadMovableMeshSettings( const KeyValues3 *pKV, void *pUnk ) = 0;
	// Empty in the game's meshes.
	virtual void Unk_29( void *p ) = 0;
	// Empty in the game's meshes; given the nav file's KeyValues3 block for versions 31 to 35.
	virtual void Unk_30( const KeyValues3 *pKV ) = 0;
	// Forwards to Unk_SaveNavGenHullParams.
	virtual void Unk_31( KeyValues3 *pKV ) const = 0;
	// Forwards to Unk_LoadNavGenHullParams.
	virtual void Unk_32( const KeyValues3 *pKV ) = 0;
	// Empty in the game's meshes; called by BuildFromNavGen.
	virtual void Unk_33( void *p ) = 0;
	// NavGenParams.HullParams: the Name, FlowMap_Enabled and FlowMap_NodeMaxRadius of every
	// hull in m_NavGenParams.
	virtual void Unk_LoadNavGenHullParams( const KeyValues3 *pKV ) = 0;
	virtual void Unk_SaveNavGenHullParams( KeyValues3 *pKV ) const = 0;

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
	void *m_pUnk30;

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
	void *m_pUnk68;
	// A copy of a frame counter, -1 while Update runs.
	int16 m_nUnk70;

public:
	CUtlString m_NavFilename;

private:
	// A vector freed by the destructor.
	uint8 m_Unk80[ 0x18 ];

public:
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
	// Three vectors, the second collecting dock areas, followed by a pointer back to the mesh.
	// No class name or accessor identifies the object.
	uint8 m_Unk3D0[ 0x48 ];
	CNavMesh *m_pUnk418;

public:
	// Loaded from scripts/nav_hulls.vdata; null when that file is missing.
	CNavHullVDataFileManager *m_pHullVDataFileManager;
};

#endif // NAV_MESH_H
