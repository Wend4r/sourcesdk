//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_SELECTOR_H
#define ANIMGRAPHNODE_SELECTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmIDBasedSelectorNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 5 > m_optionNodeIndices;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 5 > m_optionIDs;
		int16 m_nParameterNodeIdx;
		int16 m_nFallbackNodeIdx;
		bool m_bIgnoreInvalidOptions;
	};

	CUtlLeanVectorFixedGrowable< CNmPoseNode *, 5 > m_optionNodes;
	CNmIDValueNode *m_pParameterNode;
	CNmPoseNode *m_pFallbackNode;
	CNmPoseNode *m_pSelectedNode;
	int32 m_selectedOptionIdx;
};

abstract_class CNmParameterizedSelectorNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 8 > m_optionNodeIndices;
		CUtlLeanVectorFixedGrowable< uint8, 8 > m_optionWeights;
		int16 m_parameterNodeIdx;
		bool m_bIgnoreInvalidOptions;
		bool m_bHasWeightsSet;
	};

	CUtlLeanVectorFixedGrowable< CNmPoseNode *, 5 > m_optionNodes;
	CNmFloatValueNode *m_pParameterNode;
	CNmPoseNode *m_pSelectedNode;
	int32 m_selectedOptionIdx;
};

abstract_class CNmSelectorNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 8 > m_optionNodeIndices;
		CUtlLeanVectorFixedGrowable< int16, 8 > m_conditionNodeIndices;
	};

	CUtlLeanVectorFixedGrowable< CNmPoseNode *, 5 > m_optionNodes;
	CUtlLeanVectorFixedGrowable< CNmBoolValueNode *, 5 > m_conditions;
	CNmPoseNode *m_pSelectedNode;
	int32 m_selectedOptionIdx;
};
#endif // ANIMGRAPHNODE_SELECTOR_H
