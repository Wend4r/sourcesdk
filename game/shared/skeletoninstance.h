//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The skeleton instance: a scene node with a model, its bones and attachments.
//
//=============================================================================//

#ifndef SKELETONINSTANCE_H
#define SKELETONINSTANCE_H

#ifdef _WIN32
#pragma once
#endif

#include "gamescenenode.h"
#include "tier0/utlsymbollarge.h"
#include "tier1/utldelegate.h"
#include "tier0/interpolatedvar.h"
#include "mathlib/transform.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "scenesystem/isceneanimatableproceduralbonetransforms.h"

#include "schemacompiler/schemaon.h"

class CFeModel;
class IAttachmentOverride;
class IBoneTransformModifier;
class IBoneTransformOverride;
class IPhysAggregateInstance;
class IPhysicsSoftbody;
class IRenderBoundsWorldSpaceOverride;
struct InterpolatedAnimGraph2State_t;
struct MeshGroupMaskInfo_t;

typedef IPhysicsSoftbody *HPhysicsSoftbody;
DECLARE_POINTER_HANDLE( SceneObjectUpdaterHandle_t );

//-----------------------------------------------------------------------------
// The animated bone data of a model state. The game declares a constructor and a destructor; they
// make the class non-POD, so that a derived class continues in its tail padding on GCC as well.
//-----------------------------------------------------------------------------
class ALIGN16 CAnimationState
{
public:
	CAnimationState();
	~CAnimationState();

	CStrongHandle< InfoForResourceTypeCModel > m_hModel;
	// g_pMemAlloc block, freed only when m_bOwnsMemory is set.
	void *m_pData;
	// Bit array of the bones with readable data.
	uint32 *m_pReadableBones;
	bool m_bOwnsMemory;
	bool m_bHasNonTransformAnimationData;
	int32 m_pAnimationDataCount[ 3 ];
} ALIGN16_POST;

//-----------------------------------------------------------------------------
// The bones in parent space after composition; the interpolated arrays are allocated by the
// client.
//-----------------------------------------------------------------------------
class CCompositeParentSpaceState : public CAnimationState
{
public:
	struct InterpolatedBoneTransform_t;

	float *m_pInterpolatedBoneTransformWeights;
	InterpolatedBoneTransform_t *m_pInterpolatedBoneTransforms;
	bool m_bHasValidBoneTransformData;
	// True after construction.
	bool m_bShouldAllocateBoneTransforms;
	bool m_bShouldUseInterpolatedBoneTransforms;
	// Dota declares it "int : 8"; a plain byte keeps it at the same place on MSVC.
	uint8 m_transformType;
};

//-----------------------------------------------------------------------------
// The world space bone and attachment transforms of a model state.
//-----------------------------------------------------------------------------
class CSimulationState
{
public:
	// m_nTotalTransformCount transforms, identity until computed.
	CTransform *m_pTransforms;
	// Bit array of the valid transforms.
	uint32 *m_pReadableBones;
	// Bones plus attachments.
	int32 m_nTotalTransformCount;
	int32 m_nBoneCount;
	int32 m_nAttachmentCount;
};

//-----------------------------------------------------------------------------
// A captured cloth state, interpolated by the client.
//-----------------------------------------------------------------------------
class ALIGN16 CClothSnapshotState
{
public:
	const CFeModel *m_pFeModel;
	HPhysicsSoftbody m_hSoftbody;
	// Named by what it does: the softbody's transform at capture (Dota keeps an origin and angles
	// here).
	CTransform m_AbsTransform;
	float m_flTime;
	// Named by what it does: the tick of the capture.
	int32 m_nTickCount;
	CUtlVector< VectorAligned > m_NodePos;
	// Named by what they do: the node index of every control of the cloth model and the
	// transforms of those nodes; the second transform array is filled the same way as the first.
	CUtlVector< uint32 > m_CtrlNodeIndices;
	CUtlVector< CTransform > m_CtrlTransforms;
	CUtlVector< CTransform > m_CtrlTransformsPrev;
} ALIGN16_POST;

//-----------------------------------------------------------------------------
// The model of a skeleton instance and everything computed from it. Embedded in CSkeletonInstance
// (the game's member is a NetworkVar_m_modelState subclass that overrides NetworkStateChanged and
// NetworkStateChangedLog). Server layout; the client's is larger.
//-----------------------------------------------------------------------------
schema class CModelState
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	// Empty unless the state is a networked member.
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data );
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data );
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo );

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	CLASS_USES_KV3TRANSFER_VIRTUAL( CModelState );
	virtual const char *GetClassName() const;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const;

public:
	noschema CAnimationState m_animationState;
	noschema CCompositeParentSpaceState m_compositeParentSpaceState;
	noschema CSimulationState m_simulationState;
	CStrongHandle< InfoForResourceTypeCModel > m_hModel;
	CUtlSymbolLarge m_ModelName;
	// Named by what it does: set together with m_hNewModel; PostDataUpdate then switches to that
	// model and clears both.
	noschema bool m_bHasNewModel;
	noschema CWeakHandle< InfoForResourceTypeCModel > m_hNewModel;
	// Named by what it does: the model the bone count and the bone flags were last computed for.
	noschema CWeakHandle< InfoForResourceTypeCModel > m_hCachedModel;
	// m_nTotalBoneCount CModelSkeleton::BoneFlags_t, including the flags the children need.
	noschema uint32 *m_pBoneFlagsIncludingChildren;
	noschema uint32 m_nBoneFlagsAssociatedWithCloth;
	noschema SceneObjectUpdaterHandle_t m_hSceneObjectUpdater;
	IPhysAggregateInstance *m_pVPhysicsAggregate; META( MPhysPtr );
	float32 m_flRootBoneOffset_x;
	float32 m_flRootBoneOffset_y;
	float32 m_flRootBoneOffset_z;
	uint8 m_nRootBoneOffsetResetSerialNumber;
	bool m_bClientClothCreationSuppressed;
	noschema HPhysicsSoftbody m_hCloth2;
	// Named by what the code does: the constructor sets them to -1 and 0 and empties the vector
	// (the destructor frees it); nothing else in the server or client binaries touches them.
	noschema int32 m_nUnreferencedIndex;
	noschema uint16 m_nUnreferencedCount;
	noschema CUtlVector< uint8 > m_UnreferencedList;
	// The three interpolated variables below are allocated by the client only.
	noschema CInterpolatedVarProcedural< CCompositeParentSpaceState > *m_p_iv_compositeParentSpaceState;
	noschema CClothSnapshotState m_ClothSnapshot;
	noschema CInterpolatedVarProceduralUsingOwnerLerp< CClothSnapshotState, CModelState > *m_p_iv_clothState;
	// Named by the type it interpolates.
	noschema CInterpolatedVarProcedural< InterpolatedAnimGraph2State_t > *m_p_iv_animGraph2State;
	uint8 m_nAnimStateNoInterpSerialNumber;
	uint64 m_MeshGroupMask;
	// Named by what it does: the interpolated variable of m_MeshGroupMask.
	noschema CInterpolatedVarProcedural< MeshGroupMaskInfo_t > m_iv_meshGroupMask;
	// Networked as CNetworkUtlVectorBase< int32 >.
	CUtlVector< int32 > m_nBodyGroupChoices;
	// Named by what it does: the skeleton bone of every physics part; the inverse of the next.
	noschema CUtlVector< int16 > m_PhysicsBoneToSkeletonBone;
	noschema CUtlVector< int16 > m_SkeletonBoneToPhysicsBone;
	noschema int16 m_nTotalBoneCount;
	int8 m_nIdealMotionType;
	int8 m_nForceLOD;
	int8 m_nClothUpdateFlags;
	noschema bool m_bHierarchyBoneFlagsDirty;
	// Set by the client from the model's cloth data.
	noschema bool m_bHasCloth;
	noschema bool m_bHasProceduralBones;
	noschema bool m_bHasFlexProceduralBones;
	// Named by what the code does: cleared by the constructor, read nowhere.
	noschema bool m_bUnreferencedFlag;
	// Named by what it does: the client leaves m_MeshGroupMask alone while it is set.
	noschema bool m_bMeshGroupMaskLocked;
	// Named by what it does: bit 0 set by the client with the softbody, bit 1 cleared after the
	// hierarchy bone flags are rebuilt.
	noschema uint8 m_nClothStateFlags;
	// Named by what it does: set from the "allow_flatten_hierarchy" prop data of a physics prop
	// and passed to the physics creation.
	noschema bool m_bAllowFlattenHierarchy;
};

//-----------------------------------------------------------------------------
// A scene node with a model: bones, attachments, hitboxes. Embedded in
// CBodyComponentSkeletonInstance. Server layout; the client's is larger.
//
// The second base adds a slot to the primary table on Linux only (the override of
// SetBoneTransformsInSceneObject); MSVC keeps it in the second table.
//-----------------------------------------------------------------------------
schema class CSkeletonInstance : public CGameSceneNode, public ISceneAnimatableProceduralBoneTransforms
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CSkeletonInstance );
	const CNetworkSerializerClassInfo *GetSerializerClassInfo() const override;
	~CSkeletonInstance() override;

	void AddedToEntityDatabase( CEntityInstance *pOwner ) override;
	void Connect( const char *pszName, CEntityInstance *pOwner ) override;
	void Spawn( CEntityInstance *pOwner, const char *pszName, const CEntityKeyValues *pKeyValues ) override;
	void Activate( CEntityInstance *pOwner ) override;
	void UpdateOnRemove( CEntityInstance *pOwner ) override;
	void OnSetDormant( CEntityInstance *pOwner, EntityDormancyType_t prevDormancyType, EntityDormancyType_t newDormancyType ) override;
	CSkeletonInstance *GetSkeletonInstance() override;
	const CSkeletonInstance *GetSkeletonInstance() const override;
	void InvalidatePhysicsRecursive( uint32 nChangeFlags ) override;
	void SetAnimationParentSpaceStateDirty() override;
	void SetCompositeParentSpaceStateDirty() override;
	void SetCompositeWorldSpaceStateDirty() override;
	CUtlString DescribeHierarchyAttachType( CUtlStringToken nAttachName ) const override;
	void PerformInvalidatePhysicsRecursive( uint32 nChangeFlags ) override;
	void EndHierarchicalAttachment() override;
	void OnHierarchyChanged() override;
	// Adds m_pController to the list.
	void PostInstanceSpawn( CUtlVector< CSkeletonAnimationController * > &controllers ) override;
	void PostDataUpdate( DataUpdateType_t updateType, CUtlVector< CSkeletonAnimationController * > &controllers ) override;
	void OnRestore( CUtlVector< CSkeletonAnimationController * > &controllers ) override;
	HierarchyType_t DetermineHierarchyAttachType( CUtlStringToken nAttachName ) const override;
	bool IsBoneMergeFlexAttachType( CUtlStringToken nAttachName ) const override;
	void NetworkStateChanged( const NetworkStateChanged_t &data ) override;
	void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) override;
	void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) override;

	void SetBoneTransformsInSceneObject( CSceneAnimatableObject *pSceneObject, uint8 nLOD ) override;

public:
	CModelState m_modelState;
	bool m_bUseParentRenderBounds; META( MNotSaved );
	bool m_bDisableSolidCollisionsForHierarchy;
	bool m_bDirtyMotionType; META( MNotSaved );
	bool m_bIsGeneratingLatchedParentSpaceState; META( MNotSaved );
	// Set and cleared by the physics game system when it queues the skeleton for its kinematic
	// and its hierarchy joint updates.
	noschema bool m_bMarkedForKinematicUpdate;
	noschema bool m_bMarkedForHierarchyJointUpdate;
	// Named by what it does: while set (or while a global switch is on), a model change does not
	// rebuild the entity's physics.
	noschema bool m_bKeepPhysicsOnModelChange;
	// Named by what it does: set while the attachment transforms are being computed under
	// m_calcAbsPositionMutex.
	noschema bool m_bComputingAttachments;
	CUtlStringToken m_materialGroup;
	uint8 m_nHitboxSet;
	noschema CSkeletonAnimationController *m_pController;
	noschema IAttachmentOverride *m_pAttachmentOverride;
	noschema IBoneTransformOverride *m_pBoneTransformOverride;
	noschema IBoneTransformModifier *m_pBoneTransformModifier;
	// Named by what it does: an interface whose first virtual returns a bone mask that the bone
	// setup adds to the procedurally set bones; like the two above, it keeps the world transform
	// updates going.
	noschema void *m_pBoneMaskSource;
	// Named by what it does: a handle released through an engine interface (slot 31) in
	// UpdateOnRemove.
	noschema void *m_hEngineRegistration;
	// Named by what the code does: zeroed by the constructor, read nowhere.
	noschema uint32 m_nUnreferencedValue;
	// Named by what they do: the hitbox position history (a ring of four samples with time and
	// origin) and the cached hitbox set, a count followed by the entries.
	noschema void *m_pHitboxHistory;
	noschema int32 *m_pCachedHitboxes;
	noschema IRenderBoundsWorldSpaceOverride *m_pRenderBoundsOverride;
	// CModelSkeleton::BoneFlags_t of the valid bones of each state; FLAG_ALL_BONE_FLAGS (0xFFFFF)
	// at construction.
	noschema uint32 m_animationParentSpaceValidBoneFlags;
	noschema uint32 m_compositeParentSpaceValidBoneFlags;
	noschema uint32 m_compositeWorldSpaceValidBoneFlags;
	bool m_bForceServerConstraintsEnabled;
	// Named by what it does: an index into a list of the model (-1: none), or -2 for the entry
	// named by m_sCustomModelEntry; used when the composite parent space state is rebuilt.
	noschema int32 m_nModelEntryIndex;
	noschema CUtlString m_sCustomModelEntry;
	// Named by what the code does: the constructor zeroes these; the destructor frees the vector;
	// nothing else in the server binaries touches them.
	noschema bool m_bUnreferencedFlag;
	noschema uint32 m_nUnreferencedPair[ 2 ];
	noschema uint32 m_nUnreferencedCount;
	noschema CUtlVector< uint8 > m_UnreferencedList;
};

#include "schemacompiler/schemaoff.h"

#endif // SKELETONINSTANCE_H
