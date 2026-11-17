//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_BOOLS_H
#define ANIMGRAPHNODE_BOOLS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmAndNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 4 > m_conditionNodeIndices;
	};

	CUtlLeanVectorFixedGrowable< CNmBoolValueNode *, 4 > m_conditionNodes;
	bool m_result;
};

abstract_class CNmNotNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
	};

	CNmBoolValueNode *m_pInputValueNode;
	bool m_result;
};

abstract_class CNmOrNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 4 > m_conditionNodeIndices;
	};

	CUtlLeanVectorFixedGrowable< CNmBoolValueNode *, 4 > m_conditionNodes;
	bool m_result;
};
#endif // ANIMGRAPHNODE_BOOLS_H
