//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_ROOTMOTIONOVERRIDE_H
#define ANIMGRAPHNODE_ROOTMOTIONOVERRIDE_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "animlib/tasksystem/animtaskposepool.h"
#include "tier0/platform.h"

abstract_class CNmRootMotionOverrideNode : public CNmPassthroughNode
{
public:
	enum class OverrideFlags_t : uint8
	{
		AllowMoveX = 0,
		AllowMoveY = 1,
		AllowMoveZ = 2,
		AllowFacingPitch = 3,
		ListenForEvents = 4,
	};

	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		int16 m_desiredMovingVelocityNodeIdx;
		int16 m_desiredFacingDirectionNodeIdx;
		int16 m_linearVelocityLimitNodeIdx;
		int16 m_angularVelocityLimitNodeIdx;
		int16 m_enabledNodeIdx;
		float m_maxLinearVelocity;
		float m_maxAngularVelocityRadians;
		CNmBitFlags m_overrideFlags;
	};

	CNmVectorValueNode *m_pDesiredMovingVelocityNode;
	CNmVectorValueNode *m_pDesiredFacingDirectionNode;
	CNmFloatValueNode *m_pLinearVelocityLimitNode;
	CNmFloatValueNode *m_pAngularVelocityLimitNode;
	CNmBoolValueNode *m_pEnabledNode;
	float m_blendTime;
	float m_desiredBlendDuration;
	NmBlendState_t m_blendState;
	bool m_isFirstUpdate;
};
#endif // ANIMGRAPHNODE_ROOTMOTIONOVERRIDE_H
