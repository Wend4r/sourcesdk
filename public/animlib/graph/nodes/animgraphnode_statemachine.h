//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_STATEMACHINE_H
#define ANIMGRAPHNODE_STATEMACHINE_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

class CNmStateNode;
class CNmTransitionNode;

abstract_class CNmStateMachineNode : public CNmPoseNode
{
public:
	struct TransitionDefinition_t
	{
		int16 m_nTargetStateIdx;
		int16 m_nConditionNodeIdx;
		int16 m_nTransitionNodeIdx;
		bool m_bCanBeForced;
	};

	struct StateDefinition_t
	{
		int16 m_nStateNodeIdx;
		int16 m_nEntryConditionNodeIdx;
		CUtlLeanVectorFixedGrowable< CNmStateMachineNode::TransitionDefinition_t, 5 > m_transitionDefinitions;
	};

	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< CNmStateMachineNode::StateDefinition_t, 5 > m_stateDefinitions;
		int16 m_nDefaultStateIndex;
	};

	struct TransitionInfo_t
	{
		CNmTransitionNode *m_pTransitionNode;
		CNmBoolValueNode *m_pConditionNode;
		int16 m_targetStateIdx;
		bool m_canBeForced;
	};

	struct StateInfo_t
	{
		CNmStateNode *m_pStateNode;
		CNmBoolValueNode *m_pEntryConditionNode;
		CUtlLeanVectorFixedGrowable< TransitionInfo_t, 5 > m_transitions;
		bool m_hasForceableTransitions;
	};

	CUtlLeanVectorFixedGrowable< StateInfo_t, 10 > m_states;
	uint64 m_transitioningStates[ 2 ];
	CNmTransitionNode *m_pActiveTransition;
	int16 m_activeStateIndex;
	int16 m_requestShutdownConditionsStateIndex;
};
#endif // ANIMGRAPHNODE_STATEMACHINE_H
