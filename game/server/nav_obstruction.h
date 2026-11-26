//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Navigation obstacles (entities and edit volumes that block or mark nav areas) and
//          the managers that apply them to the nav mesh.
//
//=============================================================================//

#ifndef NAV_OBSTRUCTION_H
#define NAV_OBSTRUCTION_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlrbtree.h"
#include "entityhandle.h"
#include "schemasystem/schematypes.h"
#include "navlib/nav_pathfind.h"
#include "navlib/nav_interfaces.h"

class CNavArea;
class Vector;
class QAngle;
struct Extent;

//-----------------------------------------------------------------------------
// Applies the obstacles of one kind to the nav mesh (CNavObstructionManager,
// CNavObstacleAvoidMgr, CNavObstacleConnectionMgr, CNavObstacleSplitMgr,
// CNavTacticalSearchAnnotationMgr). No virtual destructor. CNavObstructionManager forwards
// every method to the managers it owns. The CNavMesh methods named below call the method of
// their m_pObstructionManager.
//-----------------------------------------------------------------------------
abstract_class INavObstructionManager : public INavMovableMeshListener
{
public:
	// CNavMesh::Update. The managers update their obstacles unless a boolean convar is set
	virtual void Update() = 0;
	// The same steps as Update under the opposite test of that convar; no caller found
	virtual void Stub_3() = 0;
	virtual void DebugDraw() const = 0;
	// CNavMesh::ResetNavMesh: the managers empty their hash tables
	virtual void ResetNavMesh() = 0;
	// CNavMesh::OnMeshActivated / OnMeshDeactivated
	virtual void OnMeshActivated() = 0;
	virtual void OnMeshDeactivated() = 0;
	// Empty in most managers; CNavObstructionManager resets the same member manager in both. The
	// nav space calls OnNavSpaceActivated when it becomes the active one and OnNavSpaceDeactivated
	// when another replaces it (CNavSpace::OnActivated / OnDeactivated). Named by what they do
	virtual void OnNavSpaceActivated() = 0;
	virtual void OnNavSpaceDeactivated() = 0;
	// CNavMesh::OnRoundRestart
	virtual void OnRoundRestart() = 0;
	// CNavMesh::RemoveNavArea and RemoveInvalidNavAreas: the areas leave the mesh
	virtual void RemoveNavArea( int nCount, CNavArea **ppAreas ) = 0;
	// CNavMesh::AddNavArea / AddStaticNavArea with bNotifyDock, for docks (NAV_AREA_DOCK) only
	virtual void AddNavArea( CNavArea *pArea ) = 0;
	// An area was split into newAreas
	virtual void OnNavAreaSplit( const CNavArea &area, const CUtlVector< CNavArea * > &newAreas ) = 0;
	// CNavObstructionManager calls OnMeshDeactivated first, then the managers empty their hash
	// tables; no caller found
	virtual void Stub_14() = 0;
	// The managers empty their hash tables (the same body as ResetNavMesh in most); no caller
	// found
	virtual void Stub_15() = 0;
	// The runtime generation removes the areas inside a tile
	virtual void NavGenRt_RemoveAreasInsideTile2d( const Extent &extent ) = 0;
};

//-----------------------------------------------------------------------------
// An obstacle of the nav mesh (CNavObstacleEntity, CNavEditObstacle, CRagdollPropNavObstacle).
// A schema class.
//-----------------------------------------------------------------------------
abstract_class INavObstacle
{
public:
	enum NavObstacleType_t : uint32
	{
		NAV_OBSTACLE_TYPE_INVALID = ( uint32 )-1,
		NAV_OBSTACLE_TYPE_NONE = 0,
		NAV_OBSTACLE_TYPE_AVOID,
		NAV_OBSTACLE_TYPE_CONN,
		NAV_OBSTACLE_TYPE_BLOCK,
		NAV_OBSTACLE_TYPE_PERMANENT_BLOCK,
	};

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual ~INavObstacle() {}
	virtual int GetId() const = 0;
	virtual void WorldSpaceSurroundingBounds( Vector *pMins, Vector *pMaxs ) const = 0;
	virtual void WorldSpaceOrientedBounds( Vector *pMins, Vector *pMaxs, Vector *pOrigin, QAngle *pAngles ) const = 0;
	// Empty by default; CNavObstacleEntity draws its oriented box and its bounds on the debug
	// overlay for flDuration. Named by what it does
	virtual void DrawDebug( float flDuration ) const = 0;
	// NAV_OBSTACLE_TYPE_BLOCK by default; CPhysicsProp returns m_nNavObstacleType, CFuncNavObstruction
	// NAV_OBSTACLE_TYPE_AVOID. Named by what it does
	virtual NavObstacleType_t GetNavObstacleType() const = 0;
	// Returns 1 by default, 0 in CRagdollPropNavObstacle; CNavObstacleEntity returns a byte member
	virtual bool Stub_8() const = 0;
	// False by default; CPhysicsProp returns m_bUpdateNavWhenMoving. Named by what it does
	virtual bool ShouldUpdateNavWhenMoving() const = 0;
	// CFuncNavObstruction: not disabled; CPhysicsProp: false while attached to a reference frame,
	// m_bUpdateNavWhenMoving while awake, else true for a solid prop at least 18 units tall.
	// Named by what it does
	virtual bool IsNavAvoidanceObstacleActive() const = 0;
	// False by default; the entities test bit 0x400000 of the solid flags of their collision
	// property. Named by what it does
	virtual bool IsNavBlockingObstacle() const = 0;
	// Empty by default; the entities and CRagdollPropNavObstacle set or clear the bit
	// IsNavBlockingObstacle tests. Named by what it does
	virtual void SetNavBlockingObstacle( bool bEnable ) = 0;
	virtual float GetNavObstructionHeight() const = 0;
	// Returns a pointer member; null in CRagdollPropNavObstacle
	virtual void *Stub_14() const = 0;

public:
	// Unique among the obstacles: the constructor takes it from a global counter. The obstruction
	// managers key their tables by it
	uint64 m_nId;
};

//-----------------------------------------------------------------------------
// The obstacle of an entity (doors, func_movelinear, dynamic props, func_nav_blocker;
// CNavObstacleEntityDoor derives from it). 48 bytes. The entity creates it with the obstacle
// type and registers it with the obstruction manager.
//-----------------------------------------------------------------------------
abstract_class CNavObstacleEntity : public INavObstacle
{
public:
	// Empty in both classes
	virtual void Stub_15() = 0;

public:
	// The entity of the obstacle; the bounds, the height and the blocking flag are read from it
	CEntityHandle m_hEntity;
	// The surrounding and oriented bounds come from the scene node of the entity's body component
	// instead of its collision property; set for dynamic props. Named by what it does
	bool m_bUseSceneNodeBounds;
	// Returned by GetNavObstacleType; NAV_OBSTACLE_TYPE_BLOCK for every entity of the game
	NavObstacleType_t m_nNavObstacleType;
	// Returned by IsNavAvoidanceObstacleActive; set by the creator, CBaseDoor clears it at spawn
	// in one of its states. Named by what it holds
	bool m_bNavAvoidanceObstacleActive;
	// Returned by Stub_8; only func_movelinear sets it. Named by what it holds
	bool m_bStub8Result;
	// Added to the half extents of the oriented bounds (WorldSpaceOrientedBounds); zero unless
	// set. Named by what it does
	float m_flBoundsBloat;
	// Returned by Stub_14; the constructor sets it to null and no other write was found
	void *m_pStub14Result;
};

//-----------------------------------------------------------------------------
// Keeps which nav areas the obstacles overlap (CNavSplitBlockMgr, the second base of
// CNavObstacleSplitMgr). 192 bytes. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CNavOverlapMgr
{
public:
	// One overlap of an id and an area. Named by what it holds
	struct Overlap_t
	{
		uint64 m_nId;
		int m_nAreaId;
	};

	// The order of m_OverlapsById: by id
	class OverlapIdLess_t
	{
	public:
		bool operator()( const Overlap_t &a, const Overlap_t &b ) const { return a.m_nId < b.m_nId; }
	};

	// The order of m_OverlapsByArea: by area id, then by id
	class OverlapAreaLess_t
	{
	public:
		bool operator()( const Overlap_t &a, const Overlap_t &b ) const { return a.m_nAreaId != b.m_nAreaId ? a.m_nAreaId < b.m_nAreaId : a.m_nId < b.m_nId; }
	};

	// Recomputes the overlaps of the ids; pure in the base. CNavSplitBlockMgr updates the blocks
	// of the active nav space for each id it knows
	virtual void AddOverlaps( const CUtlVector< uint64 > &ids ) = 0;
	// Removes every overlap of the id from both trees. Named by what it does
	virtual void RemoveOverlaps( uint64 nId ) = 0;
	// Empty in the base and in CNavSplitBlockMgr; CNavObstacleSplitMgr updates the overlaps of
	// the queued ids (m_PendingIds, m_DirtyIds). Named by what it does
	virtual void UpdateOverlaps() = 0;

public:
	// The overlap object registered for each id (CNavObstacleSplitMgr::UpdateObstacle creates it
	// through a factory callback the first time the id is seen). Named by what it holds
	CUtlHashtable< uint64, IOverlapObs_t * > m_Objects;
	// The same overlaps sorted two ways: the areas of an id, the ids of an area
	CUtlRBTree< Overlap_t, OverlapIdLess_t, int > m_OverlapsById;
	CUtlRBTree< Overlap_t, OverlapAreaLess_t, int > m_OverlapsByArea;
	// The key template of the area lookups of m_OverlapsByArea: each lookup takes its id (zero
	// after construction) and supplies the area id itself, then walks from the lower bound while
	// the area matches. Zeroed whole by the constructor; the area half is never read from here.
	// Named by what it does
	Overlap_t m_AreaLookupKey;
	// Ids queued for an update: by the obstacle registration, by area and movable mesh changes,
	// and the ids whose update has to be retried. Named by what they hold
	CUtlVector< uint64 > m_QueuedIds;
	CUtlVector< uint64 > m_DirtyIds;
	CUtlVector< uint64 > m_PendingIds;
};

#endif // NAV_OBSTRUCTION_H
