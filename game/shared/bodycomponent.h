//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The body component of an entity: the owner of its scene node.
//
//=============================================================================//

#ifndef BODYCOMPONENT_H
#define BODYCOMPONENT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlvector.h"
#include "entity2/entitycomponent.h"
#include "entity2/entitynetwork.h"
#include "gamescenenode.h"
#include "skeletoninstance.h"
#include "animgraphcontroller.h"

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// The body component of an entity (CBaseEntity::m_CBodyComponent). The derived classes embed the
// scene node; this class only points at it. 120 bytes on both platforms.
//
// The class has no GetComponentName: its table ends with GetAnimationController. The derived
// classes add GetComponentName after it.
//-----------------------------------------------------------------------------
schema class CBodyComponent : public CEntityComponent
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	datamap_t *GetDataDescMap() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CBodyComponent );
	CEntityComponentHelper *GetComponentHelper() override;

	// The animation controller of the body; null unless the body runs an animation graph
	virtual CBaseAnimGraphController *GetAnimationController();

public:
	// The embedded scene node of the derived class
	CGameSceneNode *m_pSceneNode; META( MNotSaved );

	// The scene nodes registered with the body: the inline storage holds four
	noschema CUtlVectorFixedGrowable< CGameSceneNode *, 4 > m_GameSceneNodeRegistry;

	CNetworkVarChainer __m_pChainEntity; META( MNotSaved );

	noschema CEntityOwnerPtr m_pOwner;
};

//-----------------------------------------------------------------------------
// A body that is a plain scene node. 400 bytes on Windows, 416 on Linux (the size of the scene
// node differs).
//-----------------------------------------------------------------------------
schema class CBodyComponentPoint : public CBodyComponent
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	datamap_t *GetDataDescMap() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CBodyComponentPoint );
	CEntityComponentHelper *GetComponentHelper() override;
	// Null
	CBaseAnimGraphController *GetAnimationController() override;

	virtual const char *GetComponentName();

public:
	// The game's member is a NetworkVar_m_sceneNode subclass of CGameSceneNode that routes the
	// network state changes of the node to the body; its layout is the one of CGameSceneNode.
	CGameSceneNode m_sceneNode;
};

//-----------------------------------------------------------------------------
// A body that is a skeleton instance (a scene node with a model). 1248 bytes on Windows, 1264 on
// Linux.
//-----------------------------------------------------------------------------
schema class CBodyComponentSkeletonInstance : public CBodyComponent
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	datamap_t *GetDataDescMap() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CBodyComponentSkeletonInstance );
	CEntityComponentHelper *GetComponentHelper() override;

	virtual const char *GetComponentName();

public:
	// The game's member is a NetworkVar_m_skeletonInstance subclass of CSkeletonInstance that
	// routes the network state changes of the instance to the body; its layout is the one of
	// CSkeletonInstance.
	CSkeletonInstance m_skeletonInstance;
};

//-----------------------------------------------------------------------------
// The body of CBaseModelEntity: a skeleton instance body without members of its own. It keeps the
// save, load and GetComponentName of CBodyComponentSkeletonInstance.
//-----------------------------------------------------------------------------
schema class CBodyComponentBaseModelEntity : public CBodyComponentSkeletonInstance
{
public:
	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CEntityComponentHelper *GetComponentHelper() override;
	// Null
	CBaseAnimGraphController *GetAnimationController() override;
};

//-----------------------------------------------------------------------------
// The body of CBaseAnimGraph: a skeleton instance body with the animation controller. 2864 bytes
// on Windows, 2880 on Linux.
//-----------------------------------------------------------------------------
schema class CBodyComponentBaseAnimGraph : public CBodyComponentSkeletonInstance
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	datamap_t *GetDataDescMap() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CBodyComponentBaseAnimGraph );
	CEntityComponentHelper *GetComponentHelper() override;
	// &m_animationController
	CBaseAnimGraphController *GetAnimationController() override;

	const char *GetComponentName() override;

public:
	// The game's member is a NetworkVar_m_animationController subclass of
	// CBaseAnimGraphController that routes the network state changes of the controller to the
	// body; its layout is the one of CBaseAnimGraphController.
	CBaseAnimGraphController m_animationController;
};

#include "schemacompiler/schemaoff.h"

#endif // BODYCOMPONENT_H
