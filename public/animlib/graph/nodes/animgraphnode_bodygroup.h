//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_BODYGROUP_H
#define ANIMGRAPHNODE_BODYGROUP_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animevents.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "tier0/platform.h"

abstract_class CNmBodyGroupNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		int16 m_nEnabledNodeIdx;
		CNmBodyGroupEvent m_event;
	};

	CNmBoolValueNode *m_pEnabledNode;
};
#endif // ANIMGRAPHNODE_BODYGROUP_H
