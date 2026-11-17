//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_ORIENTATIONWARP_H
#define ANIMGRAPHNODE_ORIENTATIONWARP_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"

class CNmClipReferenceNode;

abstract_class CNmOrientationWarpNode : public CNmPoseNode
{
public:
	enum class AlignmentMode_t : uint8
	{
		MovementDirection = 0,
		AnimationEndFacing = 1,
	};

	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nClipReferenceNodeIdx;
		int16 m_nTargetValueNodeIdx;
		bool m_bIsOffsetNode;
		bool m_bIsOffsetRelativeToCharacter;
		bool m_bWarpTranslation;
		CNmOrientationWarpNode::AlignmentMode_t m_alignmentMode;
		CNmRootMotionData::SamplingMode_t m_samplingMode;
	};

	CNmClipReferenceNode *m_pClipReferenceNode;
	CNmValueNode *m_pTargetValueNode;
	CNmRootMotionData m_warpedRootMotion;
	bool m_shouldUpdateWarp;
	CTransform m_warpStartWorldTransform;
	float m_warpStartTime;
	Vector m_debugCharacterOffsetPosWS;
	Vector m_debugTargetDirWS;
};
#endif // ANIMGRAPHNODE_ORIENTATIONWARP_H
