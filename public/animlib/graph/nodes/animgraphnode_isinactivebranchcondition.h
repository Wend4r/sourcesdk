//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_ISINACTIVEBRANCHCONDITION_H
#define ANIMGRAPHNODE_ISINACTIVEBRANCHCONDITION_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"

abstract_class CNmIsInactiveBranchConditionNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	};

	// The definition reserves 0x30 bytes for the node, more than it takes

	bool m_result;
};
#endif // ANIMGRAPHNODE_ISINACTIVEBRANCHCONDITION_H
