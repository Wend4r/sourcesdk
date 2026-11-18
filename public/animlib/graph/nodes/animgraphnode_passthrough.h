//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_PASSTHROUGH_H
#define ANIMGRAPHNODE_PASSTHROUGH_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"

abstract_class CNmPassthroughNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nChildNodeIdx;
	};

	CNmPoseNode *m_pChildNode;
};
#endif // ANIMGRAPHNODE_PASSTHROUGH_H
