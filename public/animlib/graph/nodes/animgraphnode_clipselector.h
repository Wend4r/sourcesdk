//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_CLIPSELECTOR_H
#define ANIMGRAPHNODE_CLIPSELECTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_animationclip.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmClipSelectorNode : public CNmClipReferenceNode
{
public:
	class CDefinition : public CNmClipReferenceNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 8 > m_optionNodeIndices;
		CUtlLeanVectorFixedGrowable< int16, 8 > m_conditionNodeIndices;
	};

	CUtlLeanVectorFixedGrowable< CNmClipReferenceNode *, 5 > m_optionNodes;
	CUtlLeanVectorFixedGrowable< CNmBoolValueNode *, 5 > m_conditions;
	CNmClipReferenceNode *m_pSelectedNode;
	int32 m_selectedOptionIdx;
};
#endif // ANIMGRAPHNODE_CLIPSELECTOR_H
