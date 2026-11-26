//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Interfaces the navigation mesh calls the game through: the editor, the markup
//          manager, the entity interface, the dynamic connections, the movable mesh listener
//          and the game event listener.
//
//=============================================================================//

#ifndef NAV_INTERFACES_H
#define NAV_INTERFACES_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CNavArea;

//-----------------------------------------------------------------------------
// The nav editor (CNavEditor). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class INavEditor
{
public:
	virtual void OnNavMeshDestroy() = 0;
	virtual void Update() = 0;
	virtual void Reset( bool bUnk1, bool bUnk2 ) = 0;
	virtual void RemoveInvalid() = 0;
};

//-----------------------------------------------------------------------------
// The markup of the nav mesh (CNavMarkupManager).
//-----------------------------------------------------------------------------
abstract_class INavMarkupManager
{
public:
	virtual ~INavMarkupManager() {}
	virtual void Stub_2() = 0;
	virtual void Stub_3() = 0;
};

//-----------------------------------------------------------------------------
// The entities of the nav mesh (CNavEntityInterface).
//-----------------------------------------------------------------------------
abstract_class INavEntityInterface
{
public:
	virtual ~INavEntityInterface() {}
	// The mesh is removing the areas (CNavMesh::RemoveNavArea); passes them on to a virtual
	// method of every entity of one class
	virtual void OnDestroyArea( int nCount, CNavArea **ppAreas ) = 0;
};

//-----------------------------------------------------------------------------
// The connections that change at run time (CNavDynamicConnectionsManager).
//-----------------------------------------------------------------------------
abstract_class INavDynamicConnectionsManager
{
public:
	virtual ~INavDynamicConnectionsManager() {}
	// Set and clear two flags of the manager
	virtual void Stub_2() = 0;
	virtual void Stub_3() = 0;
	virtual void Stub_4() = 0;
};

//-----------------------------------------------------------------------------
// Told when a movable part of the mesh changes (CNavChangeMgr, and INavObstructionManager
// derives from it). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class INavMovableMeshListener
{
public:
	// Empty in CNavChangeMgr. The obstruction managers get one call per moved movable mesh with
	// a record of its id and bounds (CNavObstacleSplitMgr re-applies the obstacles inside the
	// bounds); the mesh's Update also calls it on its listener without an argument
	virtual void Stub_0() = 0;
	// The movable mesh's dormant flag changed (nav_set_movable_mesh_dormant_flag); the
	// obstruction managers re-apply or drop the obstacles on its areas. Empty in CNavChangeMgr.
	// Named by what it does
	virtual void OnMovableMeshDormantChanged( uint32 nMovableMeshId, bool bDormant ) = 0;
};

//-----------------------------------------------------------------------------
// The second base of CNavMeshGameEventListener. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class INavMeshGameEventListener
{
public:
	virtual void OnNavMeshDestroy() = 0;
};

#endif // NAV_INTERFACES_H
