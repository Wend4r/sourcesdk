//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_FOOTIK_H
#define ANIMGRAPHNODE_FOOTIK_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animblendweight.h"
#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"

abstract_class CNmFootIKNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		CGlobalSymbol m_leftEffectorBoneID;
		CGlobalSymbol m_rightEffectorBoneID;
		int16 m_nLeftTargetNodeIdx;
		int16 m_nRightTargetNodeIdx;
		int16 m_nEnabledNodeIdx;
		float m_flBlendTimeSeconds;
		NmIKBlendMode_t m_blendMode;
		bool m_bIsTargetInWorldSpace;
	};

	CNmBoolValueNode *m_pEnableNode;
	CNmTargetValueNode *m_pLeftTargetNode;
	CNmTargetValueNode *m_pRightTargetNode;
	int32 m_leftFootBoneIdx;
	int32 m_rightFootBoneIdx;
	NmBlendWeight_t m_IKWeight;
	bool m_isValidSetup;
};
#endif // ANIMGRAPHNODE_FOOTIK_H
