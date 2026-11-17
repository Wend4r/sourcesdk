//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Animation system: anim containers, activity and animation event
//          registries, animation groups and animgraph instances
//
//===========================================================================//

#ifndef IANIMATIONSYSTEM_H
#define IANIMATIONSYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <appframework/iappsystem.h>
#include <tier0/platform.h>
#include <tier0/smartptr.h>
#include <resourcesystem/resourcehandle.h>
#include <resourcesystem/resourcehandletypes.h>
#include <modellib/localflexcontroller.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <animlib/graph/animgraph_definition.h>

class CUtlBuffer;
class Vector;
class QAngle;
class CTransform;
class CModel;
class CAnimContainer;
class CAnimationDecodeContext;
class CAnimationGroup;
class CAnimationGroupResource;
class CConvertOldDiskCtx;
class CNmGraphInstance;
class IAnimGraphDebugReplay;
class IAnimationGraphBuilder;
class IAnimationGraphVisualizerRegistry;
class IAnimationHelper;
struct bone_merge_list_t;
struct EmbeddedAnimInfo_t;

using HModelWeak = CWeakHandle< InfoForResourceTypeCModel >;

typedef CSmartPtr< IAnimGraphDebugReplay > IAnimGraphDebugReplayPtr;
typedef CSmartPtr< const IAnimGraphDebugReplay > IAnimGraphDebugReplayConstPtr;

// Index of a sequence in the anim container of a model
struct HSequence
{
	int32 m_Value;
};

// Index of a registered animation event. The class has a constructor, which makes it
// a hidden return argument on Windows and a register return on Linux.
struct AnimEventIndex_t
{
	AnimEventIndex_t() : m_Value( -1 ) {}

	int32 m_Value;
};

// Returned by value the same way as AnimEventIndex_t
struct AnimationEventIndexRange_t
{
	AnimationEventIndexRange_t() : m_nFirst( 0 ), m_nEnd( 0 ) {}

	int m_nFirst;
	// One past the highest registered event index
	int m_nEnd;
};

// Describes the graph instance CreateGraphInstance creates
struct NmGraphInstanceCreateParams_t
{
	// The instance does not take a reference, the caller keeps the definition loaded
	CWeakHandle< InfoForResourceTypeCNmGraphDefinition > m_hGraphDefinition;
	// Becomes the m_userID of the instance
	int32 m_userID;
	// Name the instance is listed under, m_pszFallbackName is used when it is empty
	const char *m_pszName;
	const char *m_pszFallbackName;
	// Zero in every call of the game
	void *m_pUnknown20;
	// Becomes the m_debugName of the instance
	CUtlString m_debugName;
	// Reported back by GetDebuggableGraphInstances; the game always passes true
	bool m_bUnknown30;
	bool m_bDebuggable;
};

// One entry of GetDebuggableGraphInstances
struct NmDebuggableGraphInstance_t
{
	// -1 for a graph connected to an external graph slot
	int32 m_userID;
	CNmGraphInstance *m_pInstance;
	// The instance itself for a standalone one, otherwise the instance whose slot it is connected to
	CNmGraphInstance *m_pParentInstance;
	// Only set for a graph connected to an external graph slot
	CGlobalSymbol m_externalSlotID;
	// NULL for a graph connected to an external graph slot
	const char *m_pszName;
	// m_bUnknown30 of the create params, false for a graph connected to an external graph slot
	bool m_bUnknown28;
};

abstract_class IAnimationSystem : public IAppSystem
{
public:
	virtual CAnimContainer *CreateAnimContainer() = 0;
	// Destroys the anim container held by the model
	virtual void DeleteAnimContainer( CModel *pModel ) = 0;

	virtual void BatchAnimationDecode( CAnimationDecodeContext *pContext ) = 0;

	// Computes the root transform with ComputeRootToBoneMergeBoneTransform and ComputeBoneMergeRootTransform,
	// then forwards to PerformBoneMergePreseveRootTransform without a mask.
	// Source indices of the merge list are clamped to nSourceTransformCount
	virtual void PerformBoneMerge( HModelWeak hDestModel, int nBoneMergeCount, const bone_merge_list_t *pBoneMerge, const CTransform *pWorldSpaceSourceTransforms, int nSourceTransformCount, CTransform &destRootToWorld, int nDestTransformCount, CTransform *pParentSpaceDestTransforms, CTransform *pWorldSpaceDestTransforms ) = 0;

	// Forward to the sequence of the model's anim container; an out of range sequence falls back to the first one
	virtual float ComputeSequenceCyclesPerSecond( const CModel *pModel, HSequence hSequence, const float *poseParameter ) const = 0;
	virtual int ComputeSequenceMaxFrame( const CModel *pModel, HSequence hSequence, const float *poseParameter ) const = 0;
	virtual bool ComputeSequenceMovement( const CModel *pModel, HSequence hSequence, float flPrevCycle, float flNextCycle, const float *pPoseParameter, Vector *pVecPos, QAngle *pAngles ) const = 0;
	virtual float ComputeSequenceDuration( const CModel *pModel, HSequence hSequence, const float *pPoseParameter ) const = 0;
	virtual float ComputeSequenceTotalMovementAndDuration( const CModel *pModel, HSequence hSequence, const float *pPoseParameter, Vector *pDeltaPos ) const = 0;
	virtual bool ComputeSequenceVelocity( const CModel *pModel, HSequence hSequence, float flCycle, const float *poseParameter, Vector &vecVelocity ) const = 0;

	// Both return false when there is nothing to compute
	virtual bool ComputeBoneMergeRootTransform( int nBoneMergeCount, const bone_merge_list_t *pBoneMerge, const CTransform *pWorldSpaceSourceTransforms, const CTransform &destRootToBoneMergeBone, CTransform *pDestRootToWorld ) = 0;
	virtual bool ComputeRootToBoneMergeBoneTransform( HModelWeak hDestModel, int nBoneMergeCount, const bone_merge_list_t *pBoneMerge, int nDestTransformCount, const CTransform *pParentSpaceDestTransforms, CTransform *pDestRootToBoneMergeBone ) = 0;

	virtual const char *GetInterpolationSpewDebugBoneName( const CModel *pModel ) const = 0;

	// -1 when there is no entity to debug
	virtual void SetEntityIndexToDebug( int nEntityIndex ) = 0;
	virtual int GetEntityIndexToDebug() const = 0;

	// Activity registry
	// Fails when the name is already registered as a private activity
	virtual bool ActivityList_RegisterSharedActivity( const char *pszActivityName, int nActivityIndex ) = 0;
	// Returns the existing index when the name is already registered
	virtual int ActivityList_RegisterPrivateActivity( const char *pszActivityName ) = 0;
	// Returns -1 for an unknown name
	virtual int ActivityList_IndexForName( const char *pszActivityName ) const = 0;
	// Returns NULL for an unknown index
	virtual const char *ActivityList_NameForIndex( int activityIndex ) const = 0;
	virtual int ActivityList_HighestIndex() const = 0;

	// Writes the graph to the buffer as binary KV3 in the animgraph format
	virtual bool SaveAnimationGraphToBuffer( const IAnimationGraphBuilder *pAnimGraph, CUtlBuffer &bufferOut, CUtlString *errorMsg ) = 0;

	virtual IAnimGraphDebugReplayPtr CreateAnimGraphReplay() = 0;
	virtual IAnimGraphDebugReplayPtr LoadAnimGraphReplayFromFile( const CUtlString &filename, CUtlString *errorMsg ) = 0;
	// Writes to DEFAULT_WRITE_PATH. The replay is passed by value
	virtual bool SaveAnimGraphReplayToFile( IAnimGraphDebugReplayConstPtr pReplay, const CUtlString &filename ) = 0;

	// Returns a static object
	virtual IAnimationGraphVisualizerRegistry *GetAnimationGraphVisualizerRegistry() = 0;

	virtual void FrameUpdate() = 0;

	virtual CAnimationGroupResource *CreateAnimationGroupFromV0Data( CConvertOldDiskCtx &convertCtx, CAnimationGroup *pAnimGroupV0, const char *pName ) = 0;
	// Adds a reference and returns the same group
	virtual CAnimationGroupResource *CloneAnimGroup( CAnimationGroupResource *pGroup ) = 0;
	virtual void ReleaseAnimGroup( CAnimationGroupResource *pGroup ) = 0;

	// pBoneMask holds one bit per destination bone, NULL processes every bone.
	// The misspelled name is the original one
	virtual void PerformBoneMergePreseveRootTransform( HModelWeak hDestModel, int nBoneMergeCount, const bone_merge_list_t *pBoneMerge, const CTransform *pWorldSpaceSourceTransforms, int nSourceTransformCount, const CTransform &destRootToWorld, int nDestTransformCount, CTransform *pParentSpaceDestTransforms, CTransform *pWorldSpaceDestTransforms, const uint32 *pBoneMask ) = 0;

	// Global flex controller names; models map their local flex controllers onto these indices
	virtual int NumGlobalFlexControllers() const = 0;
	// Returns an empty string for an index out of range
	virtual const char *GetGlobalFlexControllerName( int i ) const = 0;
	virtual LocalFlexController_t FindOrAddGlobalFlexController( const char *pControllerName ) = 0;

	// Animation event registry
	// Fails when the name is already registered
	virtual bool EventList_RegisterSharedEvent( const char *pszEventName, int nEventIndex, int nEventType ) = 0;
	virtual AnimEventIndex_t EventList_RegisterPrivateEvent( const char *pszEventName, int nEventType ) = 0;
	// The index is -1 for an unknown name
	virtual AnimEventIndex_t EventList_IndexForName( const char *pszEventName ) = 0;
	virtual void EventList_IndexAndTypeForName( const char *pszEventName, int *pEventIndex, int *pEventType ) = 0;
	// Returns NULL for an unknown index
	virtual const char *EventList_NameForIndex( AnimEventIndex_t nEventIndex ) = 0;
	// Returns 0 for an unknown index
	virtual int EventList_GetEventType( AnimEventIndex_t nEventIndex ) = 0;
	virtual AnimationEventIndexRange_t EventList_IndexRange() = 0;

	virtual CAnimationGroupResource *CreateAnimationGroupFromEmbeddedBlocks( const EmbeddedAnimInfo_t &info ) = 0;

	// AnimGraph2 instances. The system keeps every instance it created in a list for the animgraph debugger.
	// Returns NULL when the graph definition is not loaded or invalid
	virtual CNmGraphInstance *CreateGraphInstance( const NmGraphInstanceCreateParams_t &params ) = 0;
	virtual void DestroyGraphInstance( CNmGraphInstance *pInstance ) = 0;

	// Lists the standalone instances that are flagged debuggable
	virtual void GetDebuggableGraphInstances( CUtlVector< NmDebuggableGraphInstance_t > &outInstances ) = 0;
	// Lists every instance of the given graph resource among the debuggable instances and the graphs
	// connected to their slots
	virtual void GetDebuggableGraphInstances( const CResourceString &graphResourceName, CUtlVector< NmDebuggableGraphInstance_t > &outInstances ) = 0;

	virtual void SetGraphInstanceDebuggable( CNmGraphInstance *pInstance, bool bDebuggable ) = 0;
	// True for an instance GetDebuggableGraphInstances would list, directly or through a graph that references it
	virtual bool IsDebuggableGraphInstance( CNmGraphInstance *pInstance ) = 0;
};

abstract_class IAnimationSystemUtils
{
public:
	virtual IAnimationHelper *CreateAnimationHelper( HModelWeak hModel, void *pModelContext ) = 0;
	virtual void DestroyAnimationHelper( IAnimationHelper *pHelper ) = 0;

	virtual ~IAnimationSystemUtils() {}
};

#endif // IANIMATIONSYSTEM_H
