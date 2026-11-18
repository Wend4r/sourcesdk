//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_SCALE_H
#define ANIMGRAPHNODE_SCALE_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "tier0/platform.h"

abstract_class CNmScaleNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		int16 m_nMaskNodeIdx;
		int16 m_nEnableNodeIdx;
	};

	CNmBoneMaskValueNode *m_pMaskNode;
	CNmBoolValueNode *m_pEnableNode;
};
#endif // ANIMGRAPHNODE_SCALE_H
