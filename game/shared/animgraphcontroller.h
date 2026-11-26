#ifndef ANIMGRAPHCONTROLLER_H
#define ANIMGRAPHCONTROLLER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/globalsymbol.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlbinaryblock.h"
#include "entityhandle.h"
#include "entitytypes.h"
#include "schemasystem/schematypes.h"
#include "kv3lib/kv3transfer_constants.h"
#include "resourcefile/resourcetype.h"
#include "resourcesystem/resourcehandle.h"
#include "animationsystem/ianimationsystem.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraph_instance.h"
#include "entity2/entitynetwork.h"
#include "networksystem/networkvar.h"
#include "gamescenenode.h"

#include "schemacompiler/schemaon.h"

class CBaseEntity;
class CEntityInstance;
class CEntityKeyValues;
class CModel;
class CNmGraphInstance;
class CAnimGraphControllerManager;
class CAnimationDecodeOp;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
class CSkeletonInstance;
class ISequenceSetup;
class ISkeletonAnimationHistoryContext;
class IAnimGraphControllerModelReloadInfo;
class KeyValues3;
struct datamap_t;

// Handle of an external graph connected to the graph of an entity, -1 for none
schema struct ExternalAnimGraphHandle_t
{
	TYPEMETA( MIsBoxedIntegerType );

	uint32 m_Value;
};

//-----------------------------------------------------------------------------
// A control parameter of an AnimGraph2 graph that a graph controller drives, looked up by its ID
// in the graph instance the controller binds to. T is the type of the value: bool, float32,
// CGlobalSymbol and so on.
//-----------------------------------------------------------------------------
template < typename T >
class CAnimGraph2ParamRefBase
{
public:
	bool IsBound() const { return m_pBoundGraphInstance != nullptr && m_index >= 0; }

	CGlobalSymbol m_ID;
	CNmGraphInstance *m_pBoundGraphInstance;
	int16 m_index;

	// A missing optional parameter is not reported; CAnimGraph2ParamOptionalRef sets it.
	// Named by the dump of the bindings ("OPTIONAL").
	bool m_bOptional;

	// Set when the controller tried to bind the parameter, whether it was found or not.
	// Named by the dump of the bindings ("BOUND").
	bool m_bBound;
};

// A parameter the controller drives when the graph has it
template < typename T >
class CAnimGraph2ParamOptionalRef : public CAnimGraph2ParamRefBase< T >
{
};

// A parameter that goes back to its default value after every update of the graph, so it
// works as a trigger. The controller keeps a list of them and resets them in PostGraphUpdate.
abstract_class IAnimGraph2ParamAutoReset
{
public:
	// Writes the default value into the node of the parameter
	virtual void Reset() = 0;
};

// A bool parameter that goes back to its default value after every update of the graph (see
// IAnimGraph2ParamAutoReset). bOptional: a missing parameter is not reported, as with
// CAnimGraph2ParamOptionalRef. The game implements the virtual method.
template < bool bOptional >
class CAnimGraph2ParamAutoResetRefBase : public CAnimGraph2ParamRefBase< bool >, public IAnimGraph2ParamAutoReset
{
public:
	virtual void Reset() override;
};

using CAnimGraph2ParamAutoResetOptionalRef = CAnimGraph2ParamAutoResetRefBase< true >;

// A parameter whose value the controller copies, before every update of the graph, from the
// graph of another entity (m_hParameterSourceEntity). Named by what it holds.
struct AnimGraphControllerCopiedParameter_t
{
	CGlobalSymbol m_ID;
	int16 m_index;
	NmGraphValueType_t m_valueType;
};

// What CSkeletonAnimationController::CreateModelReloadInfo returns. One slot on both platforms,
// no virtual destructor.
abstract_class IAnimGraphControllerModelReloadInfo
{
public:
	// The only method: clears the reload flag of the controller, rebuilds its graph bindings and
	// applies the saved parameters again
	virtual void Unk_0() = 0;
};

// What CSkeletonAnimationController::SetupSequences fills (CSimSequenceSetup). Two slots on both
// platforms, no virtual destructor.
abstract_class ISequenceSetup
{
public:
	virtual void AddAnimationDecode( float flWeight, int nSequence, const CAnimationDecodeOp *pOps, int nOpCount, const float *pPoseParameters, CEntityHandle hEntity ) = 0;
	// Marks the setup used and adds a sequence by the entity handle
	virtual void Unk_1( int nUnknown, CEntityHandle hEntity ) = 0;
};

//-----------------------------------------------------------------------------
// Base of the classes that drive the control parameters of an AnimGraph2 graph from the state
// of the game (CCS2PawnGraphController, CCS2WeaponGraphController, CDynamicPropGraphController
// and so on). The CAnimGraphControllerManager of an animated entity owns its controllers, binds
// them to the graph instance once it exists and calls PreGraphUpdate and PostGraphUpdate of each
// around every update of the graph.
//
// The game implements the virtual methods; the objects are only reached through pointers.
//-----------------------------------------------------------------------------
schema class CAnimGraphControllerBase
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext );

	// The name of the most derived class, "CAnimGraphControllerBase" here
	virtual const char *GetKV3TransferClassName() const;

	virtual ~CAnimGraphControllerBase();

	// Prints "Bindings for <class>" and every parameter with its type and state, then calls
	// PrintAdditionalBindings; graphcontroller_dumpparams runs it. Named by what it does.
	virtual void PrintBindings();

	// True when the controller drives the parameter with this ID; false here. Named by what it does.
	virtual bool HasParameter( const CGlobalSymbol &parameterID );

	// Writes the current value of the parameter with this ID into a tagged value of the graph
	// (the data, then its NmGraphValueType_t) and returns true; false here and for an ID the
	// controller does not drive. Used to copy parameters into the graph of another entity.
	// Named by what it does.
	virtual bool GetParameterValue( const CGlobalSymbol &parameterID, void *pOutValue );

	// Empty in this class and never overridden, in the server and in the client; no caller was
	// found. Named by what it does.
	virtual void EmptyControllerHook();

	// Empty here; CCS2WeaponGraphController has an empty override of its own that the override
	// of CCS2PawnGraphController calls first, as the pawn does with every parameter method of the
	// weapon controller. No caller was found. Named by what it does.
	virtual void EmptyChainedParameterHook();

	// Sets every bound parameter back to the default value of its node; empty here. Named by
	// what it does.
	virtual void ResetParametersToDefault();

	// Finds the controller of the same class for this owner and external graph in the manager or
	// creates one and adds it; null here. Named by what it does.
	virtual CAnimGraphControllerBase *FindOrCreateInManager( CAnimGraphControllerManager *pManager, CBaseEntity *pOwner, ExternalAnimGraphHandle_t hExternalGraph );

	// Called by CAnimGraphControllerManager::CreateControllerToGraphBindings once m_pGraphInstance
	// is set: stores the context, calls OnCreateGraphBindings and ClearCopiedParametersOnCreate;
	// the derived classes then bind each parameter. The context is the member of the manager
	// that precedes its pointer to the main graph instance.
	virtual void CreateGraphBindings( void *pBindingContext );

	// The manager calls it when it removes the controller while bindings exist; the derived
	// classes unbind each parameter, then ClearCopiedParametersOnDestroy runs
	virtual void DestroyGraphBindings();

	// Both clear m_copiedParameters and m_hParameterSourceEntity; CreateGraphBindings calls this
	// one, DestroyGraphBindings the next. Named by what they do.
	virtual void ClearCopiedParametersOnCreate();
	virtual void ClearCopiedParametersOnDestroy();

	// Empty, never overridden; PrintBindings calls it last. Named by its caller.
	virtual void PrintAdditionalBindings();

	// Empty, never overridden; CreateGraphBindings calls it first. Named by its caller.
	virtual void OnCreateGraphBindings();

	// A constant of the derived class: the key of the controller in the manager and the
	// "ControllerTypeID" of a saved CAnimGraphControllerPtr
	virtual uint32 GetControllerTypeID() = 0;

	// CAnimGraphControllerManager::PreGraphUpdate, before the graph is evaluated
	virtual void PreGraphUpdate();

	// After the graph is evaluated: reads the sampled events and resets m_autoResetParameters
	virtual void PostGraphUpdate();

	// The graph instance the controller binds its parameters to: the main graph of the entity,
	// or the external graph of m_hExternalGraph
	noschema CNmGraphInstance *m_pGraphInstance;

	noschema CUtlVectorFixedGrowable< AnimGraphControllerCopiedParameter_t, 2 > m_copiedParameters;

	// The CBaseAnimGraph whose controllers give the values of m_copiedParameters
	noschema CEntityHandle m_hParameterSourceEntity;

	ExternalAnimGraphHandle_t m_hExternalGraph;

	// What CreateGraphBindings received
	noschema void *m_pBindingContext;

	// The entity whose manager owns the controller, and the entity the controller works for
	// ("GraphControllerManagerOwner" and "GraphControllerOwner" of a saved CAnimGraphControllerPtr);
	// a weapon controller lives in the manager of the pawn
	noschema CBaseEntity *m_pGraphControllerManagerOwner;
	noschema CBaseEntity *m_pGraphControllerOwner;

	noschema CUtlVectorFixedGrowable< IAnimGraph2ParamAutoReset *, 8 > m_autoResetParameters;
};

// How an animation controller animates its skeleton
schema enum AnimationAlgorithm_t : uint8
{
	eInvalid = (uint8)-1,
	eNone = 0,
	eSequence,
	eAnimGraph2,
	// A secondary skeleton driven by the graph of its master
	eAnimGraph2Secondary,
	eCount,
};

schema enum AnimLoopMode_t : uint32
{
	ANIM_LOOP_MODE_INVALID = (uint32)-1,
	ANIM_LOOP_MODE_NOT_LOOPING = 0,
	ANIM_LOOP_MODE_LOOPING,
	ANIM_LOOP_MODE_USE_SEQUENCE_SETTINGS,
	ANIM_LOOP_MODE_COUNT,
};

schema enum SequenceFinishNotifyState_t : uint8
{
	eDoNotNotify = 0,
	eNotifyWhenFinished,
	eNotifyTriggered,
};

//-----------------------------------------------------------------------------
// The animation controller of a skeleton instance (CSkeletonInstance::m_pController). The scene
// node of the skeleton forwards its entity events to it.
//
// The table is declared here as a whole; CSkeletonAnimationController implements the methods
// that are not pure in the game.
//-----------------------------------------------------------------------------
schema class ISkeletonAnimationController
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
	CLASS_USES_KV3TRANSFER_VIRTUAL( ISkeletonAnimationController );
	// "CSkeletonAnimationController"; the derived controllers keep it
	virtual const char *GetClassName() const = 0;

	// Forwarded by the skeleton instance from its scene node
	virtual void Connect( CEntityInstance *pOwner ) = 0;
	virtual void AddedToEntityDatabase( CEntityInstance *pOwner ) = 0;
	virtual void Spawn( CEntityInstance *pOwner, const CEntityKeyValues *pKeyValues ) = 0;

	// Named by what it does: recomputes the AnimationAlgorithm_t of the controller from its owner
	// and model
	virtual void UpdateAnimationAlgorithm() = 0;

	virtual void Activate( CEntityInstance *pOwner, ActivateType_t activateType ) = 0;
	virtual void UpdateOnRemove( CEntityInstance *pOwner ) = 0;
	virtual void OnSetDormant( CEntityInstance *pOwner, EntityDormancyType_t prevDormancyType, EntityDormancyType_t newDormancyType ) = 0;
	virtual void PreDataUpdate( CEntityInstance *pOwner, DataUpdateType_t updateType ) = 0;
	virtual void PostDataUpdate( CEntityInstance *pOwner, DataUpdateType_t updateType ) = 0;
	virtual void OnRestore( CEntityInstance *pOwner ) = 0;

	// The skeleton instance calls it from Activate with the new model; nAnimationAlgorithm is -1
	// there, otherwise the algorithm to use
	virtual void OnNewModel( const CModel *pOldModel, const CModel *pNewModel, AnimationAlgorithm_t nAnimationAlgorithm ) = 0;

	virtual CSkeletonInstance *GetSkeletonInstance() = 0;
	virtual HSequence GetSequence() const = 0;

	// nLoopMode goes to m_nAnimLoopMode of the animation graph controller; nFlags is passed on
	// to the sequence change and not identified further
	virtual void SetSequence( HSequence hSequence, AnimLoopMode_t nLoopMode, int nFlags ) = 0;

	virtual void GetSequenceBounds( Vector *pMins, Vector *pMaxs ) const = 0;

	// Named by what it does
	virtual AnimLoopMode_t GetAnimLoopMode() const = 0;

	virtual void SetupSequences( ISequenceSetup *pSetup ) = 0;

	// Named by what it does: adds the owner to the update list of the animation graph game
	// system, or removes it when the controller does not animate (always when bForceRemove is set)
	virtual void UpdateAnimGraphUpdateList( bool bForceRemove ) = 0;

	virtual void SetupAnimationHistoryContext( ISkeletonAnimationHistoryContext *pContext ) = 0;

	// Named by what it does: allocates the object that keeps the state of the controller across a
	// reload of its model
	virtual IAnimGraphControllerModelReloadInfo *CreateModelReloadInfo() = 0;

	// Named by what it does: false in every controller class; no caller was found
	virtual bool ReturnsFalseQuery() const = 0;

	virtual AnimationAlgorithm_t GetAnimationAlgorithm() const = 0;
	virtual datamap_t *GetDataDescMap() = 0;

	// Also stores the controller in CSkeletonInstance::m_pController
	virtual void SetSkeletonInstance( CSkeletonInstance *pSkeletonInstance ) = 0;

	// Named by what it does: sends the sequence finished notification to the owner once
	// m_nNotifyState reached eNotifyTriggered
	virtual void NotifySequenceFinished() = 0;

	virtual bool IsSequenceFinished() const = 0;
	virtual float SequenceDuration( HSequence hSequence ) const = 0;
	virtual void SetPlaybackRate( float flRate ) = 0;
};

//-----------------------------------------------------------------------------
// 16 bytes on both platforms
//-----------------------------------------------------------------------------
schema class CSkeletonAnimationController : public ISkeletonAnimationController
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	const CNetworkSerializerClassInfo *GetSerializerClassInfo() const override;
	// Both empty
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CSkeletonAnimationController );
	const char *GetClassName() const override;

	// The next five are empty
	void UpdateOnRemove( CEntityInstance *pOwner ) override;
	void OnSetDormant( CEntityInstance *pOwner, EntityDormancyType_t prevDormancyType, EntityDormancyType_t newDormancyType ) override;
	void PreDataUpdate( CEntityInstance *pOwner, DataUpdateType_t updateType ) override;
	void PostDataUpdate( CEntityInstance *pOwner, DataUpdateType_t updateType ) override;
	void OnRestore( CEntityInstance *pOwner ) override;

	// m_pSkeletonInstance
	CSkeletonInstance *GetSkeletonInstance() override;

	// False and eNone
	bool ReturnsFalseQuery() const override;
	AnimationAlgorithm_t GetAnimationAlgorithm() const override;

	datamap_t *GetDataDescMap() override;
	void SetSkeletonInstance( CSkeletonInstance *pSkeletonInstance ) override;

	// Empty
	void NotifySequenceFinished() override;

public:
	CSkeletonInstance *m_pSkeletonInstance; META( MNotSaved );
};

// Second base of CBaseAnimGraphController. Its only method sets m_bSequenceFinished.
abstract_class IAnimGraphSequenceFinishedListener
{
public:
	virtual void OnSequenceFinished() = 0;
};

//-----------------------------------------------------------------------------
// A slot of CBaseAnimGraphController::m_SerializePoseRecipeAG2Slots: the serialized topology of a
// pose recipe of the graph. 64 bytes on both platforms. The table routes the network state
// changes of the slot to the owner through __m_pChainEntity.
//-----------------------------------------------------------------------------
schema struct AnimGraph2SerializedPoseRecipeSlot_t
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding();
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data );
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data );
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo );
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const;

public:
	noschema CNetworkVarChainer __m_pChainEntity;
	CUtlBinaryBlock m_topology; META( MNotSaved );
};

//-----------------------------------------------------------------------------
// The external graphs connected to the graph of a CBaseAnimGraphController. 32 bytes on both
// platforms. The members are named by what the code does.
//-----------------------------------------------------------------------------
schema class CExternalAnimGraphList
{
public:
	struct ExternalGraph_t
	{
		// Taken from CBaseAnimGraphController::m_nNextExternalGraphHandle when the graph is added
		ExternalAnimGraphHandle_t m_hExternalGraph;

		// The identifier the graph instance of the owner knows the external graph by; zero
		// until it is connected
		uint64 m_nConnectedGraphId;

		CStrongHandle< InfoForResourceTypeCNmGraphDefinition > m_hGraphDefinition;

		// The entity whose graph runs the external graph, -1 for the owner itself
		CEntityHandle m_hEntity;

		// Destroyed through the animation system when the entry goes away
		CNmGraphInstance *m_pGraphInstance;

		// The state of m_pGraphInstance saved before a reload of the graph resources and
		// restored after it
		KeyValues3 *m_pReloadSavedState;

		// 2 once the external graph is connected; 0 marks it for removal
		uint8 m_nConnectionState;

		// The graph controller created for the external graph, removed from the controller
		// manager of the owner together with the entry
		CAnimGraphControllerBase *m_pGraphController;
	};

	noschema CBaseAnimGraphController *m_pOwner;
	noschema CUtlVector< ExternalGraph_t > m_Graphs;
};

//-----------------------------------------------------------------------------
// The animation controller of CBaseAnimGraph: sequences and AnimGraph2 graphs. Embedded in
// CBodyComponentBaseAnimGraph (m_animationController). 1616 bytes on both platforms.
//
// The second base adds a slot to the primary table on Linux only (the override of
// OnSequenceFinished); MSVC keeps it in the second table.
//-----------------------------------------------------------------------------
schema class CBaseAnimGraphController : public CSkeletonAnimationController, public IAnimGraphSequenceFinishedListener
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	// An animation event of the current sequence collected between two cycles. Named by what it
	// holds; the members are named by what the code writes into them.
	struct SequenceEvent_t
	{
		// The first member of the event definition in the sequence
		int32 m_nEventType;

		// The cycle at which an instant event fires, -1 for an event with a duration
		float32 m_flCycle;

		// How far into its duration an event with a duration is, -1 for an instant event
		float32 m_flDurationFraction;

		// CAnimEventDefinition::m_EventData of the event
		const KeyValues3 *m_pEventData;

		// The flags of the event definition (client / server event bits)
		uint32 m_nEventFlags;

		// The animation layer the query was made for, INT_MIN for none
		int32 m_nAnimLayer;

		HSequence m_hSequence;
	};

	// One of the lists of m_auxiliaryState
	struct AuxiliaryTable_t
	{
		CUtlHashtable< uint64, uint64 > m_Table;
		CUtlVector< void * > m_Values;
		int32 m_nCount;
	};

	// A member the server constructs and destroys as a whole and never reads otherwise; the
	// element types are not identified. Named by its shape.
	struct AuxiliaryState_t
	{
		void *m_pFirst;
		CUtlVectorFixedGrowable< void *, 3 > m_FirstList;
		void *m_pSecond;
		CUtlVectorFixedGrowable< void *, 3 > m_SecondList;
		CUtlVectorFixedGrowable< void *, 3 > m_ThirdList;
		CUtlLeanVectorFixedGrowable< CUtlKeyValuePair< uint64, uint64 >, 10 > m_Pairs;
		int32 m_nTableCount;
		AuxiliaryTable_t m_Tables[ 3 ];
	};

	class NetworkVar_m_vecSecondarySkeletonSlotIDs;
	class NetworkVar_m_vecSecondarySkeletons;
	class NetworkVar_m_SerializePoseRecipeAG2Slots;
	class NetworkVar_m_SerializePoseRecipeAG2Dynamic;
	class NetworkVar_m_vecExternalGraphIds;
	class NetworkVar_m_vecExternalClipIds;

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	const CNetworkSerializerClassInfo *GetSerializerClassInfo() const override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CBaseAnimGraphController );

	// Empty
	void Connect( CEntityInstance *pOwner ) override;
	void AddedToEntityDatabase( CEntityInstance *pOwner ) override;
	// Sets m_nPrevAnimUpdateTick and m_flSeqStartTime
	void Spawn( CEntityInstance *pOwner, const CEntityKeyValues *pKeyValues ) override;
	void UpdateAnimationAlgorithm() override;
	void Activate( CEntityInstance *pOwner, ActivateType_t activateType ) override;
	// Disconnects and frees the external graphs
	void UpdateOnRemove( CEntityInstance *pOwner ) override;
	// Empty
	void OnSetDormant( CEntityInstance *pOwner, EntityDormancyType_t prevDormancyType, EntityDormancyType_t newDormancyType ) override;
	void PostDataUpdate( CEntityInstance *pOwner, DataUpdateType_t updateType ) override;
	void OnNewModel( const CModel *pOldModel, const CModel *pNewModel, AnimationAlgorithm_t nAnimationAlgorithm ) override;
	// The skeleton instance of the owner's body
	CSkeletonInstance *GetSkeletonInstance() override;
	HSequence GetSequence() const override;
	void SetSequence( HSequence hSequence, AnimLoopMode_t nLoopMode, int nFlags ) override;
	void GetSequenceBounds( Vector *pMins, Vector *pMaxs ) const override;
	AnimLoopMode_t GetAnimLoopMode() const override;
	void SetupSequences( ISequenceSetup *pSetup ) override;
	void UpdateAnimGraphUpdateList( bool bForceRemove ) override;
	// Empty
	void SetupAnimationHistoryContext( ISkeletonAnimationHistoryContext *pContext ) override;
	IAnimGraphControllerModelReloadInfo *CreateModelReloadInfo() override;
	bool ReturnsFalseQuery() const override;
	// eAnimGraph2Secondary while m_nSecondarySkeletonMasterCount is positive, else m_nAnimationAlgorithm
	AnimationAlgorithm_t GetAnimationAlgorithm() const override;
	datamap_t *GetDataDescMap() override;
	void NotifySequenceFinished() override;
	// m_bSequenceFinished
	bool IsSequenceFinished() const override;
	float SequenceDuration( HSequence hSequence ) const override;
	void SetPlaybackRate( float flRate ) override;

	// Empty here; the NetworkVar_m_animationController subclass of the body component routes
	// them to the body
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data );
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data );
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo );

	// Sets m_bSequenceFinished
	void OnSequenceFinished() override;

	// Advances the cycle of the sequence; true when it finished. Named by the debug output
	// ("FrameAdvance")
	virtual bool FrameAdvance();

	// m_flPlaybackRate
	virtual float GetPlaybackRate() const;

	// Named by what it does: appends the sequences that make up the current pose; the element
	// type of the output is not identified
	virtual void GatherSequences( void *pOutSequences, int *pOutInfo );

public:
	AnimationAlgorithm_t m_nAnimationAlgorithm;
	ExternalAnimGraphHandle_t m_nNextExternalGraphHandle;
	CNetworkUtlVectorBase< CGlobalSymbol, NetworkVar_m_vecSecondarySkeletonSlotIDs > m_vecSecondarySkeletonSlotIDs;
	// The handles of CBaseAnimGraph entities
	CNetworkUtlVectorBase< CEntityHandle, NetworkVar_m_vecSecondarySkeletons > m_vecSecondarySkeletons;
	int32 m_nSecondarySkeletonMasterCount;
	float32 m_flSoundSyncTime;
	uint32 m_nActiveIKChainMask;
	HSequence m_hSequence;
	GameTime_t m_flSeqStartTime;
	float32 m_flSeqFixedCycle;
	AnimLoopMode_t m_nAnimLoopMode;
	CNetworkedQuantizedFloat m_flPlaybackRate;

	// The cycles per second of m_hSequence, set on activation; the cycle advances by it times the
	// time since m_flSeqStartTime and m_flPlaybackRate
	noschema float32 m_flCachedSequenceCycleRate;

	SequenceFinishNotifyState_t m_nNotifyState;
	bool m_bNetworkedAnimationInputsChanged;
	bool m_bNetworkedSequenceChanged;
	bool m_bLastUpdateSkipped;
	bool m_bSequenceFinished;

	// Named by what it does: set by CreateModelReloadInfo; OnNewModel does nothing while it is set
	noschema bool m_bModelReloadInProgress;

	GameTick_t m_nPrevAnimUpdateTick;

	// Named by what it does: the animation events of the sequence between the previous and the
	// current cycle, rebuilt on every advance
	noschema CUtlVectorFixedGrowable< SequenceEvent_t, 16 > m_SequenceEvents;

	CStrongHandle< InfoForResourceTypeCNmGraphDefinition > m_hGraphDefinitionAG2;
	CUtlVectorEmbeddedNetworkVar< AnimGraph2SerializedPoseRecipeSlot_t, NetworkVar_m_SerializePoseRecipeAG2Slots > m_SerializePoseRecipeAG2Slots; META( MNotSaved );
	CNetworkUtlVectorBase< uint8, NetworkVar_m_SerializePoseRecipeAG2Dynamic > m_SerializePoseRecipeAG2Dynamic; META( MNotSaved );
	uint32 m_nSerializePoseRecipeAG2ActiveSlot; META( MNotSaved );
	int32 m_nSerializePoseRecipeVersionAG2; META( MNotSaved );

	// Named by what they do: while the first is set the pose recipe slots are cleared instead of
	// serialized (set when an external graph is disconnected); the second is cleared at the start
	// of every serialization
	noschema bool m_bPoseRecipeSerializationSuspended;
	noschema bool m_bPoseRecipeSerializationRequested;

	// Named by what it does: the pose recipes already serialized and their slots (32 bytes,
	// owned); a full cache evicts a random entry. Its type is not identified
	noschema void *m_pPoseRecipeSlotCache;

	int32 m_nServerGraphInstanceIteration;
	int32 m_nServerSerializationContextIteration;
	ResourceId_t m_primaryGraphId;
	CNetworkUtlVectorBase< ResourceId_t, NetworkVar_m_vecExternalGraphIds > m_vecExternalGraphIds;
	CNetworkUtlVectorBase< ResourceId_t, NetworkVar_m_vecExternalClipIds > m_vecExternalClipIds;
	CGlobalSymbol m_sAnimGraph2Identifier;
	CAnimGraph2InstancePtr m_pGraphInstanceAG2;

	noschema AuxiliaryState_t m_auxiliaryState;

	CExternalAnimGraphList m_vecExternalGraphs;

	// Named by what it does: the state of the graph instance saved before a reload of the graph
	// resources and restored after it (owned)
	noschema KeyValues3 *m_pReloadSavedGraphState;
};

#include "schemacompiler/schemaoff.h"

#endif // ANIMGRAPHCONTROLLER_H
