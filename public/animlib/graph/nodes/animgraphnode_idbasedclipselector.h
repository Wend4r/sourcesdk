//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_IDBASEDCLIPSELECTOR_H
#define ANIMGRAPHNODE_IDBASEDCLIPSELECTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_animationclip.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmIDBasedClipSelectorNode : public CNmClipReferenceNode
{
public:
	class CDefinition : public CNmClipReferenceNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 5 > m_optionNodeIndices;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 5 > m_optionIDs;
		int16 m_nParameterNodeIdx;
		int16 m_nFallbackNodeIdx;
		bool m_bIgnoreInvalidOptions;
	};

	CUtlLeanVectorFixedGrowable< CNmClipReferenceNode *, 5 > m_optionNodes;
	CNmIDValueNode *m_pParameterNode;
	CNmClipReferenceNode *m_pFallbackNode;
	CNmClipReferenceNode *m_pSelectedNode;
	int32 m_selectedOptionIdx;
};
#endif // ANIMGRAPHNODE_IDBASEDCLIPSELECTOR_H
