//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_STATECONDITIONS_H
#define ANIMGRAPHNODE_STATECONDITIONS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"

class CNmStateNode;

abstract_class CNmStateCompletedConditionNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		int16 m_nTransitionDurationOverrideNodeIdx;
		float m_flTransitionDurationSeconds;
	};

	CNmStateNode *m_pSourceStateNode;
	CNmFloatValueNode *m_pDurationOverrideNode;
	bool m_result;
};

abstract_class CNmTimeConditionNode : public CNmBoolValueNode
{
public:
	enum class ComparisonType_t : uint8
	{
		PercentageThroughState = 0,
		PercentageThroughSyncEvent = 1,
		ElapsedTime = 2,
	};

	enum class Operator_t : uint8
	{
		LessThan = 0,
		LessThanEqual = 1,
		GreaterThan = 2,
		GreaterThanEqual = 3,
	};

	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_sourceStateNodeIdx;
		int16 m_nInputValueNodeIdx;
		float m_flComparand;
		CNmTimeConditionNode::ComparisonType_t m_type;
		CNmTimeConditionNode::Operator_t m_operator;
	};

	CNmStateNode *m_pSourceStateNode;
	CNmFloatValueNode *m_pInputValueNode;
	bool m_result;
};
#endif // ANIMGRAPHNODE_STATECONDITIONS_H
