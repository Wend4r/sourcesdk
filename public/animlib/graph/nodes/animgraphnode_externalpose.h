//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_EXTERNALPOSE_H
#define ANIMGRAPHNODE_EXTERNALPOSE_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/tasksystem/animbonemasktask.h"
#include "animlib/tasksystem/animtaskposepool.h"
#include "mathlib/transform.h"
#include "resourcesystem/resourcehandle.h"
#include "tier0/platform.h"

abstract_class CNmExternalPoseNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		bool m_bShouldSampleRootMotion;
	};

	// A clip and the part of it the current update plays
	struct ClipSample_t
	{
		CStrongHandle< InfoForResourceTypeCNmClip > m_hClip;
		float m_flPreviousTime;
		float m_flCurrentTime;

		// A looped update plays previous time .. loop end time, then loop start time .. current time
		bool m_bLooped;
		float m_flLoopEndTime;
		float m_flLoopStartTime;
	};

	// The pose CNmGraphInstance::SetExternalPose() puts into the slot of the node:
	// a clip, optionally blended with a second one
	struct Pose_t
	{
		ClipSample_t m_source;
		ClipSample_t m_target;

		// 0 plays the source only, the target has to be set for anything above that
		float m_flBlendWeight;
	};

	Pose_t m_pose;
	bool m_bIsPoseSet;
	bool m_bUsesClipPose;
	CTransform m_rootMotionDelta;
	CNmBoneMaskTaskList m_boneMask;
	NmCachedPoseID_t m_externalPoseBufferID;
};
#endif // ANIMGRAPHNODE_EXTERNALPOSE_H
