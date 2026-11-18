//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph instance: the runtime state of one graph on one
//          animation controller
//
//===========================================================================//

#ifndef ANIMGRAPH_INSTANCE_H
#define ANIMGRAPH_INSTANCE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/globalsymbol.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "tier0/utlstring.h"
#include "tier0/utlhashtable.h"
#include "mathlib/vector.h"
#include "mathlib/transform.h"
#include "resourcesystem/resourcehandle.h"
#include "animlib/animlib_types.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_parameters.h"
#include "animlib/graph/animgraph_definition.h"
#include "animlib/graph/animgraph_context.h"

#include "schemacompiler/schemaon.h"

class CBaseAnimGraphController;
class CNmPoseNode;
class CNmGraphRecording;

// Tables the instance keeps to serialize the pose tasks of an update, rebuilt whenever the set
// of connected graphs or secondary skeletons changes. The engine has no upstream counterpart with
// this layout, so only its shape is known: two pointers, three inline lists of three pointers,
// an inline list of ten 16-byte entries and three pairs of a hash table and a list of its values.
struct NmTaskSerializationContext_t
{
	uint64 m_opaque[ 0x42 ];
};

// Created and destroyed by the animation system for an animation controller. The game reads and
// writes control parameters straight in the nodes, the accessors below do the same with the same
// checks. They must run on the game thread, like everything else that touches the instance.
schema class CNmGraphInstance
{
public:
	const CNmGraphDefinition *GetGraphDefinition() const { return m_hGraphDefinition.GetData(); }

	// Is this a full self-contained instance, not one a parent graph references
	bool IsStandaloneInstance() const { return m_isStandaloneGraphInstance; }

	uint32 GetUpdateID() const { return m_graphContext.m_updateID; }
	const CTransform &GetRootMotionDeltaForLastUpdate() const { return m_rootMotionDelta; }
	const CUtlVector< int16 > &GetActiveNodes() const { return m_graphContext.m_activeNodes; }

	int GetNumControlParameters() const
	{
		const CNmGraphDefinition *pDefinition = GetGraphDefinition();
		return pDefinition ? pDefinition->GetNumControlParameters() : 0;
	}

	bool IsControlParameter( int16 nodeIdx ) const
	{
		return nodeIdx >= 0 && nodeIdx < GetNumControlParameters() && nodeIdx < m_nodes.Count() && m_nodes[ nodeIdx ] != nullptr;
	}

	// Returns -1 for an ID that is not a control parameter of the graph. The game looks the ID up
	// in the lookup map of the definition, which also resolves virtual parameters that cannot be set.
	int16 GetControlParameterIndex( const CGlobalSymbol &parameterID ) const
	{
		const CNmGraphDefinition *pDefinition = GetGraphDefinition();
		if ( !pDefinition )
		{
			return -1;
		}

		int16 parameterNodeIdx = pDefinition->GetControlParameterIndex( parameterID );
		return IsControlParameter( parameterNodeIdx ) ? parameterNodeIdx : static_cast< int16 >( -1 );
	}

	CGlobalSymbol GetControlParameterID( int16 parameterNodeIdx ) const
	{
		return IsControlParameter( parameterNodeIdx ) ? GetGraphDefinition()->m_controlParameterIDs[ parameterNodeIdx ] : CGlobalSymbol();
	}

	// Returns Unknown for an index that is not a control parameter
	NmGraphValueType_t GetControlParameterType( int16 parameterNodeIdx ) const
	{
		return IsControlParameter( parameterNodeIdx ) ? m_nodes[ parameterNodeIdx ]->GetValueType() : NmGraphValueType_t::Unknown;
	}

	// The setters and getters fail when the index is not a control parameter or the parameter
	// has another type, where the game asserts

	// Rejects values that would convert to one of the types below, such as a string literal to bool
	template < class T >
	bool SetControlParameterValue( int16 parameterNodeIdx, T value ) = delete;

	bool SetControlParameterValue( int16 parameterNodeIdx, bool value )
	{
		CNmControlParameterBoolNode *pNode = GetControlParameterNode< CNmControlParameterBoolNode >( parameterNodeIdx, NmGraphValueType_t::Bool );
		if ( !pNode )
		{
			return false;
		}

		pNode->m_value = value;
		return true;
	}

	bool SetControlParameterValue( int16 parameterNodeIdx, const CGlobalSymbol &value )
	{
		CNmControlParameterIDNode *pNode = GetControlParameterNode< CNmControlParameterIDNode >( parameterNodeIdx, NmGraphValueType_t::ID );
		if ( !pNode )
		{
			return false;
		}

		pNode->m_value = value;
		return true;
	}

	// Values that are not finite or do not fit an int32 are rejected
	bool SetControlParameterValue( int16 parameterNodeIdx, float value )
	{
		CNmControlParameterFloatNode *pNode = GetControlParameterNode< CNmControlParameterFloatNode >( parameterNodeIdx, NmGraphValueType_t::Float );
		if ( !pNode || !( value > -2147483648.0f && value < 2147483648.0f ) )
		{
			return false;
		}

		pNode->m_value = value;
		return true;
	}

	// Vectors with a component that is not finite are rejected
	bool SetControlParameterValue( int16 parameterNodeIdx, const Vector &value )
	{
		CNmControlParameterVectorNode *pNode = GetControlParameterNode< CNmControlParameterVectorNode >( parameterNodeIdx, NmGraphValueType_t::Vector );
		if ( !pNode || !value.IsValid() )
		{
			return false;
		}

		pNode->m_value = value;
		return true;
	}

	bool SetControlParameterValue( int16 parameterNodeIdx, const CNmTarget &value )
	{
		CNmControlParameterTargetNode *pNode = GetControlParameterNode< CNmControlParameterTargetNode >( parameterNodeIdx, NmGraphValueType_t::Target );
		if ( !pNode )
		{
			return false;
		}

		pNode->m_value = value;
		return true;
	}

	// Sets a transform target the way the game builds one from a position and a rotation
	bool SetControlParameterValue( int16 parameterNodeIdx, const CTransform &value )
	{
		CNmTarget target;
		target.m_transform = value;
		target.m_transform.m_vPosition.w = 1.0f;
		target.m_bIsBoneTarget = false;
		target.m_bIsUsingBoneSpaceOffsets = true;
		target.m_bHasOffsets = false;
		target.m_bIsSet = true;

		return SetControlParameterValue( parameterNodeIdx, target );
	}

	bool GetControlParameterValue( int16 parameterNodeIdx, bool &value ) const
	{
		const CNmControlParameterBoolNode *pNode = GetControlParameterNode< CNmControlParameterBoolNode >( parameterNodeIdx, NmGraphValueType_t::Bool );
		if ( !pNode )
		{
			return false;
		}

		value = pNode->m_value;
		return true;
	}

	bool GetControlParameterValue( int16 parameterNodeIdx, CGlobalSymbol &value ) const
	{
		const CNmControlParameterIDNode *pNode = GetControlParameterNode< CNmControlParameterIDNode >( parameterNodeIdx, NmGraphValueType_t::ID );
		if ( !pNode )
		{
			return false;
		}

		value = pNode->m_value;
		return true;
	}

	bool GetControlParameterValue( int16 parameterNodeIdx, float &value ) const
	{
		const CNmControlParameterFloatNode *pNode = GetControlParameterNode< CNmControlParameterFloatNode >( parameterNodeIdx, NmGraphValueType_t::Float );
		if ( !pNode )
		{
			return false;
		}

		value = pNode->m_value;
		return true;
	}

	bool GetControlParameterValue( int16 parameterNodeIdx, Vector &value ) const
	{
		const CNmControlParameterVectorNode *pNode = GetControlParameterNode< CNmControlParameterVectorNode >( parameterNodeIdx, NmGraphValueType_t::Vector );
		if ( !pNode )
		{
			return false;
		}

		value = pNode->m_value;
		return true;
	}

	bool GetControlParameterValue( int16 parameterNodeIdx, CNmTarget &value ) const
	{
		const CNmControlParameterTargetNode *pNode = GetControlParameterNode< CNmControlParameterTargetNode >( parameterNodeIdx, NmGraphValueType_t::Target );
		if ( !pNode )
		{
			return false;
		}

		value = pNode->m_value;
		return true;
	}

	// The instance does not hold a reference of its own
	CWeakHandle< InfoForResourceTypeCNmGraphDefinition > m_hGraphDefinition;
	// Indexed by node index; the control parameter nodes come first
	CUtlVector< CNmGraphNode * > m_nodes;
	// One block of m_instanceRequiredMemory bytes that every node is constructed in
	CUtlLeanVector< uint8 > m_allocatedInstanceMemory;
	CNmPoseNode *m_pRootNode;
	// The ID the creator passed in, also the m_graphUserID of the context
	int32 m_userID;
	bool m_isStandaloneGraphInstance;
	// Cleared on creation, nothing that was examined sets it
	bool m_isExternalGraph;
	// IApplication::IsInToolsMode() at creation, enables the graph log and the root motion debugger
	bool m_bInToolsMode;

	CNmGraphContext m_graphContext;
	NmTaskSerializationContext_t m_taskSerializationContext;
	// Incremented every time m_taskSerializationContext is rebuilt
	int32 m_nTaskSerializationContextIteration;
	bool m_taskSerializationRequiresUpdate;
	bool m_externalGraphStateChanged;
	CUtlVectorFixedGrowable< const CNmSkeleton *, 3 > m_secondarySkeletons;

	CTransform m_rootMotionDelta;
	// Zeroed on creation and whenever the graph state is reset
	int32 m_nUnknown390;
	// Name of the instance in the animgraph debugger
	CUtlString m_debugName;
	NmGraphDebugMode_t m_debugMode;
	CNmGraphRecording *m_pRecording;

private:
	template < class T >
	T *GetControlParameterNode( int16 parameterNodeIdx, NmGraphValueType_t nType ) const
	{
		if ( GetControlParameterType( parameterNodeIdx ) != nType )
		{
			return nullptr;
		}

		return static_cast< T * >( m_nodes[ parameterNodeIdx ] );
	}
};

// Member of an animation controller that holds its graph instance, NULL until the controller
// has a loaded graph definition
schema class CAnimGraph2InstancePtr
{
public:
	CNmGraphInstance *Get() const { return m_pInstance; }

	CBaseAnimGraphController *m_pController;
	CNmGraphInstance *m_pInstance;
};

#include "schemacompiler/schemaoff.h"

#endif // ANIMGRAPH_INSTANCE_H
