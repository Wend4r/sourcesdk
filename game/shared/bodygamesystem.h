//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system of the body components: the extra post data update
//			list of the scene nodes and, in tools mode, the state of the scene nodes
//			whose model is reloaded. Factory "BodyGameSystem".
//
//=============================================================================//

#ifndef BODYGAMESYSTEM_H
#define BODYGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "gamescenenode.h"
#include "mathlib/transform.h"
#include "resourcesystem/itoolsresourcelistener.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "tier0/globalsymbol.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"

class CGameSceneNode;

//-----------------------------------------------------------------------------
// Reallocating factory, 96 bytes on both platforms. Names: the log messages of the class (NotifyResourceStatusChange,
// NotifyResourcePreReload, "RC=" for the reload count); the rest is named by what the
// code does. libserver.so repeats the overrides of the two listeners at the end of the
// main table (slots 66 and 67).
//-----------------------------------------------------------------------------
class CBodyGameSystem : public IGameSystem, public IToolsResourceListener, public IToolsResourcePreReloadListener
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CBodyGameSystem();

	// In tools mode registers the two listeners with the resource system
	GS_EVENT( GameInit );
	// In tools mode unregisters the two listeners
	GS_EVENT( GameShutdown );
	// The four handlers below are empty (one folded body on Linux)
	GS_EVENT( GameActivate );
	GS_EVENT( GameDeactivate );
	GS_EVENT( GameFrameBoundary );
	GS_EVENT( OutOfGameFrameBoundary );

	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	// IToolsResourceListener: for every model (vmdl) reloaded (RESOURCE_LOAD_RELOAD) counts
	// m_nModelReloadCount down; at zero sorts m_SavedSceneNodes, restores the scene nodes from
	// them and empties the list
	virtual void NotifyResourceStatusChange( const ResourceStatusChange_t *pChanges, int64 nCount ) override;
	// IToolsResourcePreReloadListener: for a model (vmdl) saves the state of every scene node
	// that uses it into m_SavedSceneNodes and counts m_nModelReloadCount up
	virtual void NotifyResourcePreReload( ResourceHandle_t hResource, ResourceType_t nResourceType ) override;

public:
	// Named by what the code does: a body group of the model and the choice the scene node had
	struct BodyGroupChoice_t
	{
		CGlobalSymbol m_Name;
		int m_nChoice;
	};

	// Named by what the code does: the state of one scene node saved before its model is
	// reloaded, 128 bytes (120 used, aligned to 16 by the transform)
	struct SavedSceneNode_t
	{
		CGameSceneNodeHandle m_hSceneNode;
		// The model of the node's skeleton instance (a reference is held)
		CStrongHandle< InfoForResourceTypeCModel > m_hModel;
		CGameSceneNodeHandle m_hParent;
		// CGameSceneNode::m_hierarchyAttachName of the node
		CUtlStringToken m_hierarchyAttachName;
		// CGameSceneNode::m_nHierarchicalDepth of the node
		int m_nHierarchicalDepth;
		// Origin and scale (in the w of the position), and rotation of the node; with
		// m_bTransformFromPhysics the transform of its physics aggregate instead
		CTransform m_Transform;
		// From a virtual method (slot 24) of an object the skeleton instance holds; its slot 0
		// is called after the node was restored
		void *m_pRestoreTarget;
		// Set while the node still has to be restored
		bool m_bPending;
		bool m_bTransformFromPhysics;
		// The body group choices of the entity of the node, restored by name
		CUtlVector< BodyGroupChoice_t > m_BodyGroupChoices;
	};

	// Scene nodes whose post data update is done after the others (a flag of the node keeps
	// it from being added twice)
	CUtlVector< CGameSceneNode * > m_extraPostDataUpdates;
	CUtlVector< SavedSceneNode_t > m_SavedSceneNodes;
	// Models announced by NotifyResourcePreReload and not yet reloaded ("RC=" of the log)
	int m_nModelReloadCount;
};

#endif // BODYGAMESYSTEM_H
