//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_TWOBONEIK_H
#define ANIMGRAPHNODE_TWOBONEIK_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animblendweight.h"
#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"

abstract_class CNmTwoBoneIKNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		CGlobalSymbol m_effectorBoneID;
		int16 m_nEffectorTargetNodeIdx;
		int16 m_nEnabledNodeIdx;
		float m_flBlendTimeSeconds;
		NmIKBlendMode_t m_blendMode;
		bool m_bIsTargetInWorldSpace;
		float m_flChainRotationWeight;
	};

	CNmTargetValueNode *m_pEffectorTargetNode;
	CNmBoolValueNode *m_pEnableNode;
	int32 m_effectorBoneIdx;
	NmBlendWeight_t m_IKWeight;
	bool m_isValidSetup;
};
#endif // ANIMGRAPHNODE_TWOBONEIK_H
