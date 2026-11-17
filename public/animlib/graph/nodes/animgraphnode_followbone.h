//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_FOLLOWBONE_H
#define ANIMGRAPHNODE_FOLLOWBONE_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"

abstract_class CNmFollowBoneNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		CGlobalSymbol m_bone;
		CGlobalSymbol m_followTargetBone;
		int16 m_nEnabledNodeIdx;
		NmFollowBoneMode_t m_mode;
	};

	CNmBoolValueNode *m_pEnabledNode;
	int32 m_boneIdx;
	int32 m_followTargetBoneIdx;
};
#endif // ANIMGRAPHNODE_FOLLOWBONE_H
