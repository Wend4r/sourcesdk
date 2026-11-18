//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_PARAMETERIZEDCLIPSELECTOR_H
#define ANIMGRAPHNODE_PARAMETERIZEDCLIPSELECTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_animationclip.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmParameterizedClipSelectorNode : public CNmClipReferenceNode
{
public:
	class CDefinition : public CNmClipReferenceNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 8 > m_optionNodeIndices;
		CUtlLeanVectorFixedGrowable< uint8, 8 > m_optionWeights;
		int16 m_parameterNodeIdx;
		bool m_bIgnoreInvalidOptions;
		bool m_bHasWeightsSet;
	};

	CUtlLeanVectorFixedGrowable< CNmClipReferenceNode *, 5 > m_optionNodes;
	CNmFloatValueNode *m_pParameterNode;
	CNmClipReferenceNode *m_pSelectedNode;
	int32 m_selectedOptionIdx;
};
#endif // ANIMGRAPHNODE_PARAMETERIZEDCLIPSELECTOR_H
