//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game scene node: the transform and hierarchy part of an entity's body component.
//
//=============================================================================//

#ifndef GAMESCENENODE_H
#define GAMESCENENODE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/threadtools.h"
#include "tier0/utlstring.h"
#include "tier0/utlstringtoken.h"
#include "tier0/utlvector.h"
#include "mathlib/vector.h"
#include "mathlib/vectorws.h"
#include "mathlib/transformws.h"
#include "entityhandle.h"
#include "entitytypes.h"
#include "kv3lib/kv3transfer_constants.h"
#include "schemasystem/schematypes.h"

#include "schemacompiler/schemaon.h"

class CEntityInstance;
class CEntityKeyValues;
class CGameSceneNode;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
class CSkeletonAnimationController;
class CSkeletonInstance;
struct BoneMergeInfo_t;
struct FlexBoneMergeInfo_t;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;
struct TransformHistory_t;
template < class T > class CInterpolatedVarProcedural;
template < class T, class OWNER > class CInterpolatedVarProceduralUsingOwnerLerp;

//-----------------------------------------------------------------------------
// How a scene node is attached to its parent: CGameSceneNode::m_nHierarchyType, and the value of
// DetermineHierarchyAttachType() for an attachment name ("bonemerge" and "bonemergeflex" give
// HIERARCHY_BONE_MERGE, "absorigin" or no name HIERARCHY_ABSORIGIN, a model attachment
// HIERARCHY_ATTACHMENT, a bone HIERARCHY_BONE).
//-----------------------------------------------------------------------------
enum HierarchyType_t : uint32
{
	HIERARCHY_NONE = 0,
	HIERARCHY_BONE_MERGE = 1,
	HIERARCHY_ATTACHMENT = 2,
	HIERARCHY_ABSORIGIN = 3,
	HIERARCHY_BONE = 4,
	HIERARCHY_TYPE_COUNT = 5,
};

//-----------------------------------------------------------------------------
// A networked float with a quantized float encoder. The two 16-bit halves after the value are a
// copy of the field's QuantizedFloatEncoderReg_t: the encoder index and the field name symbol.
// The owner may use the top bit for its own flag: CGameSceneNode keeps there, in m_vecOrigin's
// m_vecX, whether the origin is networked unflattened (Dota's m_bUnflattened).
//-----------------------------------------------------------------------------
struct CNetworkedQuantizedFloat
{
	float32 m_Value;
	uint16 m_nEncoder;
	uint16 m_nFieldName : 15;
	uint16 m_bUnflattened : 1;
};

//-----------------------------------------------------------------------------
// The object an embedded network variable reports its changes to, found at a fixed offset from
// the variable.
//-----------------------------------------------------------------------------
abstract_class INetworkStateChangedRouter
{
public:
	virtual void NetworkStateChanged( void *pField, int32 nOffset, int32 nArrayIndex, int32 nPathIndex ) = 0;
	virtual void NetworkStateChangedLog( const char *pszFieldName ) = 0;
};

//-----------------------------------------------------------------------------
// A networked origin split into world cells and quantized offsets inside the cell. Not a schema
// class and not polymorphic: changes go to the INetworkStateChangedRouter that lies
// m_nStateChangedRouterOffset bytes from the vector (0x7FFF: none).
//-----------------------------------------------------------------------------
class CNetworkOriginCellCoordQuantizedVector
{
public:
	Vector m_Value;
	// Set by SetNetworkStateChangedRouter (asserts 0 < offset < INT16_MAX).
	int16 m_nStateChangedRouterOffset;
	// Named by what the code does: zeroed together with the cells by the constructor; no other
	// access in either server binary.
	uint16 m_nUnusedCell;
	uint16 m_cellX;
	uint16 m_cellY;
	uint16 m_cellZ;
	// Bit per axis, set while that coordinate is beyond the configured world bounds.
	uint16 m_nOutsideWorld;
	// Offsets inside the cell, "origin_cell_offset" encoder.
	CNetworkedQuantizedFloat m_vecX;
	CNetworkedQuantizedFloat m_vecY;
	CNetworkedQuantizedFloat m_vecZ;
};

//-----------------------------------------------------------------------------
// A networked reference to a scene node: the owner entity and the node's name inside the owner's
// body component. The game's CGameSceneNode::m_hParent is a NetworkVar_m_hParent subclass that
// overrides NetworkStateChanged and NetworkStateChangedLog.
//-----------------------------------------------------------------------------
schema class CGameSceneNodeHandle
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();

	// Empty unless the handle is a networked member.
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data );
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data );
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo );

	CLASS_USES_KV3TRANSFER_VIRTUAL( CGameSceneNodeHandle );
	virtual const char *GetClassName() const;

	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const;

public:
	CEntityHandle m_hOwner;
	CUtlStringToken m_name;
};

//-----------------------------------------------------------------------------
// The scene node of an entity: local and world transform, parent / child links and the
// hierarchical attachment. Embedded in CBodyComponentPoint, and the base of CSkeletonInstance.
//
// Server layout. The client build of this class has more members (the wrapped local origin,
// angles and scale) and so a different size and different offsets after m_flAbsScale.
//-----------------------------------------------------------------------------
schema class CGameSceneNode
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	CLASS_USES_KV3TRANSFER_VIRTUAL( CGameSceneNode );
	virtual const char *GetClassName() const;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const;
	virtual ~CGameSceneNode();

	// Forwarded by the body component from the owner entity.
	virtual void AddedToEntityDatabase( CEntityInstance *pOwner );
	virtual void Connect( const char *pszName, CEntityInstance *pOwner );
	virtual void Spawn( CEntityInstance *pOwner, const char *pszName, const CEntityKeyValues *pKeyValues );
	virtual void Activate( CEntityInstance *pOwner );
	virtual void UpdateOnRemove( CEntityInstance *pOwner );
	virtual void OnSetDormant( CEntityInstance *pOwner, EntityDormancyType_t prevDormancyType, EntityDormancyType_t newDormancyType );

	// nullptr unless the node is a CSkeletonInstance.
	virtual CSkeletonInstance *GetSkeletonInstance();
	virtual const CSkeletonInstance *GetSkeletonInstance() const;

	// Accumulates the flags into m_nCachedInvalidatePhysicsRecursiveFlags (and the children);
	// bits 0-2 mark the position, angles and scale dirty, bits 0x50 the animation state.
	virtual void InvalidatePhysicsRecursive( uint32 nChangeFlags );
	virtual void SetAnimationParentSpaceStateDirty();
	virtual void SetCompositeParentSpaceStateDirty();
	virtual void SetCompositeWorldSpaceStateDirty();
	// Named by what it does: the text form of an attachment name for debug output ("none",
	// "absorigin"; the skeleton instance adds "bonemerge", "bonemergeflex", "att(%d:%s)",
	// "bone(%d:%s)").
	virtual CUtlString DescribeHierarchyAttachType( CUtlStringToken nAttachName ) const;
	virtual void PerformInvalidatePhysicsRecursive( uint32 nChangeFlags );
	// Named by what it does (the non-virtual of the same name in Dota 2018 does the same): frees
	// the bone merge lists, resets m_nParentAttachmentOrBone and unlinks the node from its parent.
	virtual void EndHierarchicalAttachment();
	// Resolves m_hParent and m_name into a parent node and starts the hierarchical attachment, or
	// detaches. Dota declared a bool parameter; the CS2 bodies do not read one.
	virtual void OnHierarchyChanged();
	virtual void PreInstanceSpawn();
	virtual void PostInstanceSpawn( CUtlVector< CSkeletonAnimationController * > &controllers );
	virtual void PostDataUpdate( DataUpdateType_t updateType, CUtlVector< CSkeletonAnimationController * > &controllers );
	virtual void OnRestore( CUtlVector< CSkeletonAnimationController * > &controllers );
	virtual HierarchyType_t DetermineHierarchyAttachType( CUtlStringToken nAttachName ) const;
	// Named by what it does: false here; true in CSkeletonInstance for "bonemergeflex".
	virtual bool IsBoneMergeFlexAttachType( CUtlStringToken nAttachName ) const;

	// Empty unless the node is a networked member (CBodyComponentPoint::m_sceneNode).
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data );
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data );
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo );

public:
	// The router of m_vecOrigin: forwards its changes to the node with the offset of m_vecOrigin.
	class Cm_vecOriginInitializer : public INetworkStateChangedRouter
	{
	public:
		void NetworkStateChanged( void *pField, int32 nOffset, int32 nArrayIndex, int32 nPathIndex ) override;
		void NetworkStateChangedLog( const char *pszFieldName ) override;
	};

	CTransformWS m_nodeToWorld; META( MNotSaved );
	CEntityInstance *m_pOwner; META( MNotSaved );
	CGameSceneNode *m_pParent; META( MNotSaved );
	CGameSceneNode *m_pChild; META( MNotSaved );
	CGameSceneNode *m_pNextSibling; META( MNotSaved );
	// g_pMemAlloc blocks built by BuildBoneMergeWork: the bone merge list, and for
	// m_bBoneMergeFlex the flex controller pairs.
	noschema BoneMergeInfo_t *m_pBoneMergeList;
	noschema FlexBoneMergeInfo_t *m_pFlexBoneMergeList;
	// Allocated by the client only ("LocalScale" and the root-to-parent history); always null on
	// the server.
	noschema CInterpolatedVarProcedural< float > *m_p_iv_localScale;
	noschema CInterpolatedVarProceduralUsingOwnerLerp< TransformHistory_t, CGameSceneNode > *m_p_iv_rootToParent;
	CGameSceneNodeHandle m_hParent;
	CNetworkOriginCellCoordQuantizedVector m_vecOrigin;
	noschema Cm_vecOriginInitializer m_m_vecOriginInitializer;
	QAngle m_angRotation;
	float32 m_flScale;
	VectorWS m_vecAbsOrigin;
	QAngle m_angAbsRotation;
	float32 m_flAbsScale;
	int16 m_nParentAttachmentOrBone; META( MNotSaved );
	bool m_bDebugAbsOriginChanges; META( MNotSaved );
	bool m_bDormant;
	bool m_bForceParentToBeNetworked;
	uint8 m_bDirtyHierarchy : 1; META( MNotSaved );
	uint8 m_bDirtyBoneMergeInfo : 1; META( MNotSaved );
	uint8 m_bNetworkedPositionChanged : 1; META( MNotSaved );
	uint8 m_bNetworkedAnglesChanged : 1; META( MNotSaved );
	uint8 m_bNetworkedScaleChanged : 1; META( MNotSaved );
	uint8 m_bWillBeCallingPostDataUpdate : 1; META( MNotSaved );
	uint8 m_bBoneMergeFlex : 1; META( MNotSaved );
	// A 2-bit uint8 field does not straddle a byte: it starts the next byte on both compilers.
	uint8 m_nLatchAbsOrigin : 2; META( MNotSaved );
	uint8 m_bDirtyBoneMergeBoneToRoot : 1; META( MNotSaved );
	uint8 m_nHierarchicalDepth; META( MNotSaved );
	uint8 m_nHierarchyType; META( MNotSaved );
	uint8 m_nDoNotSetAnimTimeInInvalidatePhysicsCount; META( MNotSaved );
	CUtlStringToken m_name;
	// Set when the position, angles or scale change; the world transform is recomputed under
	// m_calcAbsPositionMutex.
	noschema bool m_bDirtyAbsTransform;
	// Set by SetCompositeWorldSpaceStateDirty.
	noschema bool m_bDirtyAttachments;
	noschema CThreadFastMutex m_calcAbsPositionMutex;
	CUtlStringToken m_hierarchyAttachName;
	float32 m_flClientLocalScale;
	// Interlocked OR of the InvalidatePhysicsRecursive flags not yet performed.
	noschema CInterlockedUInt m_nCachedInvalidatePhysicsRecursiveFlags;
};

#include "schemacompiler/schemaoff.h"

#endif // GAMESCENENODE_H
