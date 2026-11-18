//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_TRANSITION_H
#define ANIMGRAPHNODE_TRANSITION_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/tasksystem/animbonemasktask.h"
#include "animlib/tasksystem/animtaskposepool.h"
#include "tier0/platform.h"

class CNmStateNode;

abstract_class CNmTransitionNode : public CNmPoseNode
{
public:
	enum class TransitionOptions_t : uint8
	{
		None = 0,
		ClampDuration = 1,
		Synchronized = 2,
		MatchSourceTime = 3,
		MatchSyncEventIndex = 4,
		MatchSyncEventID = 5,
		MatchSyncEventPercentage = 6,
		PreferClosestSyncEventID = 7,
		MatchTimeInSeconds = 8,
		OffsetTimeInSeconds = 9,
	};

	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nTargetStateNodeIdx;
		int16 m_nDurationOverrideNodeIdx;
		int16 m_timeOffsetOverrideNodeIdx;
		int16 m_startBoneMaskNodeIdx;
		float m_flDuration;
		NmPercent_t m_boneMaskBlendInTimePercentage;
		float m_flTimeOffset;
		CNmBitFlags m_transitionOptions;
		int16 m_targetSyncIDNodeIdx;
		NmEasingOperation_t m_blendWeightEasing;
		NmRootMotionBlendMode_t m_rootMotionBlend;
	};

	enum class SourceType_t : int32
	{
		State = 0,
		Transition,
		CachedPose,
		OffState,
	};

	CNmPoseNode *m_pSourceNode;
	CNmStateNode *m_pTargetNode;
	CNmFloatValueNode *m_pDurationOverrideNode;
	CNmFloatValueNode *m_pTimeOffsetOverrideNode;
	CNmBoneMaskValueNode *m_pStartBoneMaskNode;
	CNmIDValueNode *m_pTargetSyncIDNode;
	CNmSyncTrack m_syncTrack;
	float m_transitionProgress;
	float m_transitionDuration;
	float m_syncEventOffset;
	float m_blendWeight;
	float m_blendedDuration;
	NmCachedPoseID_t m_cachedPoseBufferID;
	SourceType_t m_sourceType;
	CNmBoneMaskTaskList m_boneMaskTaskList;
	bool m_bGraphStateRestored;
	int16 m_rootMotionActionIdxSource;
	int16 m_rootMotionActionIdxTarget;
};
#endif // ANIMGRAPHNODE_TRANSITION_H
