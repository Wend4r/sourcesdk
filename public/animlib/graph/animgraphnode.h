//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 runtime nodes: the base classes every node of a graph
//          instance derives from
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_H
#define ANIMGRAPHNODE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/transform.h"
#include "schemasystem/schematypes.h"
#include "animlib/graph/animgraph_valuetypes.h"

#include "schemacompiler/schemaon.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNmGraphContext;
class CNmRecordedGraphState;
class CNmInstantiationContext;
class CNmSyncTrack;
struct NmSyncTrackTime_t;
struct NmSyncTrackTimeRange_t;

enum class NmInstantiationOptions_t : int32
{
	// Construct the node in the memory of the instance
	CreateNode = 0,
	// A derived definition has constructed the node already, only set its node pointers
	NodeAlreadyCreated,
};

// Range of events in the sampled events buffer of the graph context
struct NmSampledEventRange_t
{
	int16 m_startIdx;
	int16 m_endIdx;
};

struct NmGraphPoseNodeResult_t
{
	bool HasRegisteredTasks() const { return m_taskIdx != -1; }

	// Index of the pose task the node registered, -1 when it produced no pose
	int8 m_taskIdx;
	CTransform m_rootMotionDelta;
	NmSampledEventRange_t m_sampledEventRange;
};

// Runtime state of one node of a graph instance. The instance owns its nodes and creates them
// from the node definitions of the graph, so these classes are never constructed here.
abstract_class CNmGraphNode
{
public:
	// Immutable compiled settings of the node, shared by every instance of the graph
	class CDefinition
	{
	public:
		virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
		virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
		virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
		// Name of the most derived definition class, such as "CNmNotNode::CDefinition"
		virtual const char *GetKV3TransferClassName() const = 0;

		virtual ~CDefinition() {}

		// Factory method: constructs the node and sets its node pointers.
		// The other nodes may not be constructed yet, so it must not touch them
		virtual void InstantiateNode( const CNmInstantiationContext &context, NmInstantiationOptions_t options ) const = 0;
		virtual bool RequiresPostInstantiationStage() const = 0;
		virtual void PostInstantiateNode( const CNmInstantiationContext &context ) const = 0;
		// Size and alignment of the runtime node, the loader lays the nodes of an instance out with them
		virtual void GetNodeMemoryRequirements( size_t &nSize, size_t &nAlignment ) const = 0;

		// Index of the node in the graph, graphs are limited to 32k nodes
		int16 m_nNodeIdx;
	};

	virtual ~CNmGraphNode() {}

	virtual bool IsValid() const = 0;
	virtual NmGraphValueType_t GetValueType() const = 0;
	int16 GetNodeIndex() const { return m_pDefinition->m_nNodeIdx; }

	bool IsInitialized() const { return m_initializationCount > 0; }
	// Initialization is reference counted, only the first call reaches InitializeInternal
	virtual void Initialize( CNmGraphContext &context ) = 0;

	// Was the node evaluated by the graph update with this ID
	bool IsNodeActive( uint32 updateID ) const { return m_lastUpdateID == updateID; }

	virtual void RecordGraphState( CNmRecordedGraphState &outState ) = 0;
	virtual bool RestoreGraphState( const CNmRecordedGraphState &inState ) = 0;

	virtual void InitializeInternal( CNmGraphContext &context ) = 0;
	virtual void ShutdownInternal( CNmGraphContext &context ) = 0;

	const CDefinition *m_pDefinition;
	// -1 while the node is shut down
	uint32 m_lastUpdateID;
	int32 m_initializationCount;
};

// A node that produces a pose
abstract_class CNmPoseNode : public CNmGraphNode
{
public:
	class CDefinition : public CNmGraphNode::CDefinition
	{
	};

	virtual const CNmSyncTrack &GetSyncTrack() const = 0;
	// Initializes the node at a specific time
	virtual void InitializeInternal( CNmGraphContext &context, const NmSyncTrackTime_t &initialTime ) = 0;
	// Runs synchronized when an update range is given, otherwise advances by the delta time of the context
	virtual NmGraphPoseNodeResult_t Update( CNmGraphContext &context, const NmSyncTrackTimeRange_t *pUpdateRange ) = 0;
	// Empty in every node of the shipped game, so the type of the draw context is not known
	virtual void DrawDebug( CNmGraphContext &graphContext, void *pDrawContext ) = 0;

	// In seconds
	float m_duration;
	// Both are percentages through the duration, clamped to it
	float m_currentTime;
	float m_previousTime;
};

// A node that produces a value instead of a pose
abstract_class CNmValueNode : public CNmGraphNode
{
public:
	class CDefinition : public CNmGraphNode::CDefinition
	{
	};

	// Named by what it does: passes a value-initialized value of the node's type to SetValueInternal
	// (every value node shares the implementation of its value type)
	virtual void ResetValue() = 0;
	// Both take a pointer to a value of the type the node holds
	virtual void GetValueInternal( CNmGraphContext &context, void *pOutValue ) = 0;
	virtual void SetValueInternal( const void *pInValue ) = 0;
};

abstract_class CNmBoolValueNode : public CNmValueNode
{
public:
	class CDefinition : public CNmValueNode::CDefinition
	{
	};
};

abstract_class CNmIDValueNode : public CNmValueNode
{
public:
	class CDefinition : public CNmValueNode::CDefinition
	{
	};
};

abstract_class CNmFloatValueNode : public CNmValueNode
{
public:
	class CDefinition : public CNmValueNode::CDefinition
	{
	};
};

abstract_class CNmVectorValueNode : public CNmValueNode
{
public:
	class CDefinition : public CNmValueNode::CDefinition
	{
	};
};

abstract_class CNmTargetValueNode : public CNmValueNode
{
public:
	class CDefinition : public CNmValueNode::CDefinition
	{
	};
};

abstract_class CNmBoneMaskValueNode : public CNmValueNode
{
public:
	class CDefinition : public CNmValueNode::CDefinition
	{
	};
};

#include "schemacompiler/schemaoff.h"

#endif // ANIMGRAPHNODE_H
