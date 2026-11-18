//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_TARGETWARP_H
#define ANIMGRAPHNODE_TARGETWARP_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/animrootmotionwarp.h"
#include "animlib/graph/animgraphnode.h"
#include "mathlib/transform.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"

class CNmClipReferenceNode;

abstract_class CNmTargetWarpNode : public CNmPoseNode
{
public:
	enum class TargetUpdateRule_t : uint8
	{
		None = 0,
		Recalculate = 1,
		Offset = 2,
		RecalculateOrOffset = 3,
	};

	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nClipReferenceNodeIdx;
		int16 m_nTargetValueNodeIdx;
		CNmRootMotionData::SamplingMode_t m_samplingMode;
		CNmTargetWarpNode::TargetUpdateRule_t m_targetUpdateRule;
		bool m_bAlignWithTargetAtLastWarpEvent;
		float m_flSamplingPositionErrorThresholdSq;
		float m_flMaxTangentLength;
		float m_flLerpFallbackDistanceThreshold;
		float m_flTargetUpdateDistanceThreshold;
		float m_flTargetUpdateAngleThresholdRadians;
		CGlobalSymbol m_alignmentBoneID;
	};

	enum class InternalState_t : int32
	{
		RequiresInitialUpdate = 0,
		AllowUpdates,
		Completed,
		Failed,
	};

	CNmClipReferenceNode *m_pClipReferenceNode;
	CNmTargetValueNode *m_pTargetValueNode;
	CNmRootMotionData::SamplingMode_t m_samplingMode;
	InternalState_t m_internalState;
	CTransform m_warpRootTargetTransform;
	CTransform m_alignmentBoneRootTargetOffset;
	CNmRootMotionWarpContext m_warpContext;
	bool m_performWarp;
	int32 m_alignmentBoneIdx;
};
#endif // ANIMGRAPHNODE_TARGETWARP_H
