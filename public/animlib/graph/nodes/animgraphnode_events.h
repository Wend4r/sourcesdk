//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_EVENTS_H
#define ANIMGRAPHNODE_EVENTS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

class CNmStateNode;

abstract_class CNmCurrentSyncEventIDNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
	};

	CNmStateNode *m_pSourceStateNode;
	CGlobalSymbol m_result;
};

abstract_class CNmCurrentSyncEventNode : public CNmFloatValueNode
{
public:
	enum class InfoType_t : uint8
	{
		IndexAndPercentage = 0,
		IndexOnly = 1,
		PercentageOnly = 2,
	};

	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		CNmCurrentSyncEventNode::InfoType_t m_infoType;
	};

	CNmStateNode *m_pSourceStateNode;
	float m_result;
};

abstract_class CNmFloatCurveEventNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		CGlobalSymbol m_eventID;
		int16 m_nDefaultNodeIdx;
		float m_flDefaultValue;
		CNmBitFlags m_eventConditionRules;
	};

	CNmFloatValueNode *m_pDefaultValueNode;
	float m_result;
};

abstract_class CNmFootEventConditionNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		NmFootPhaseCondition_t m_phaseCondition;
		CNmBitFlags m_eventConditionRules;
	};

	const CNmStateNode *m_pSourceStateNode;
	bool m_result;
};

abstract_class CNmFootstepEventIDNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		CNmBitFlags m_eventConditionRules;
	};

	const CNmStateNode *m_pSourceStateNode;
	CGlobalSymbol m_result;
};

abstract_class CNmFootstepEventPercentageThroughNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		NmFootPhaseCondition_t m_phaseCondition;
		CNmBitFlags m_eventConditionRules;
	};

	const CNmStateNode *m_pSourceStateNode;
	float m_result;
};

abstract_class CNmGraphEventConditionNode : public CNmBoolValueNode
{
public:
	struct Condition_t
	{
		CGlobalSymbol m_eventID;
		NmGraphEventTypeCondition_t m_eventTypeCondition;
	};

	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		CNmBitFlags m_eventConditionRules;
		CUtlVectorFixedGrowable< CNmGraphEventConditionNode::Condition_t, 5 > m_conditions;
	};

	const CNmStateNode *m_pSourceStateNode;
	bool m_result;
};

abstract_class CNmIDEventConditionNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		CNmBitFlags m_eventConditionRules;
		CUtlVectorFixedGrowable< CGlobalSymbol, 5 > m_eventIDs;
	};

	const CNmStateNode *m_pSourceStateNode;
	bool m_result;
};

abstract_class CNmIDEventNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		CNmBitFlags m_eventConditionRules;
		CGlobalSymbol m_defaultValue;
	};

	const CNmStateNode *m_pSourceStateNode;
	CGlobalSymbol m_value;
};

abstract_class CNmIDEventPercentageThroughNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		CNmBitFlags m_eventConditionRules;
		CGlobalSymbol m_eventID;
	};

	const CNmStateNode *m_pSourceStateNode;
	float m_result;
};

abstract_class CNmSyncEventIndexConditionNode : public CNmBoolValueNode
{
public:
	enum class TriggerMode_t : uint8
	{
		ExactlyAtEventIndex = 0,
		GreaterThanEqualToEventIndex = 1,
	};

	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nSourceStateNodeIdx;
		CNmSyncEventIndexConditionNode::TriggerMode_t m_triggerMode;
		int32 m_syncEventIdx;
	};

	CNmStateNode *m_pSourceStateNode;
	bool m_result;
};

abstract_class CNmTransitionEventConditionNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		CGlobalSymbol m_requireRuleID;
		CNmBitFlags m_eventConditionRules;
		int16 m_nSourceStateNodeIdx;
		NmTransitionRuleCondition_t m_ruleCondition;
	};

	const CNmStateNode *m_pSourceStateNode;
	bool m_result;
};
#endif // ANIMGRAPHNODE_EVENTS_H
