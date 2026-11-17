//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_IDS_H
#define ANIMGRAPHNODE_IDS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmIDComparisonNode : public CNmBoolValueNode
{
public:
	enum class Comparison_t : uint8
	{
		Matches = 0,
		DoesntMatch = 1,
	};

	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		CNmIDComparisonNode::Comparison_t m_comparison;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 4 > m_comparisionIDs;
	};

	CNmIDValueNode *m_pInputValueNode;
	bool m_result;
};

abstract_class CNmIDSelectorNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 5 > m_conditionNodeIndices;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 5 > m_values;
		CGlobalSymbol m_defaultValue;
	};

	CUtlLeanVectorFixedGrowable< CNmBoolValueNode *, 5 > m_conditionNodes;
	CGlobalSymbol m_value;
};

abstract_class CNmIDSwitchNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	public:
		int16 m_nSwitchValueNodeIdx;
		int16 m_nTrueValueNodeIdx;
		int16 m_nFalseValueNodeIdx;
		CGlobalSymbol m_falseValue;
		CGlobalSymbol m_trueValue;
	};

	CNmBoolValueNode *m_pSwitchValueNode;
	CNmIDValueNode *m_pTrueValueNode;
	CNmIDValueNode *m_pFalseValueNode;
	CGlobalSymbol m_value;
};

abstract_class CNmIDToFloatNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		float m_defaultValue;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 5 > m_IDs;
		CUtlLeanVectorFixedGrowable< float, 5 > m_values;
	};

	CNmIDValueNode *m_pInputValueNode;
	float m_value;
};
#endif // ANIMGRAPHNODE_IDS_H
