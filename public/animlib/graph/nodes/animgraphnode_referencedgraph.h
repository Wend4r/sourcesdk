//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_REFERENCEDGRAPH_H
#define ANIMGRAPHNODE_REFERENCEDGRAPH_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

class CNmGraphInstance;
class CNmReferencedGraphNode;

abstract_class CNmIsExternalGraphSlotFilledNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nExternalGraphNodeIdx;
	};

	CNmReferencedGraphNode *m_pExternalGraphNode;
	bool m_result;
};

abstract_class CNmReferencedGraphNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nReferencedGraphIdx;
		int16 m_nFallbackNodeIdx;
	};

	CNmGraphInstance *m_pGraphInstance;
	CNmPoseNode *m_pFallbackNode;
	CUtlLeanVectorFixedGrowable< CNmValueNode *, 10 > m_parameterMapping;
};
#endif // ANIMGRAPHNODE_REFERENCEDGRAPH_H
