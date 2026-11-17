//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_POSE_H
#define ANIMGRAPHNODE_POSE_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "mathlib/piecewisecurve.h"
#include "resourcesystem/resourcehandle.h"
#include "tier0/platform.h"

abstract_class CNmAnimationPoseNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nPoseTimeValueNodeIdx;
		int16 m_nDataSlotIdx;
		Range_t m_inputTimeRemapRange;
		float m_flUserSpecifiedTime;
		bool m_bUseFramesAsInput;
	};

	CNmFloatValueNode *m_pPoseTimeValue;
	CWeakHandle< InfoForResourceTypeCNmClip > m_pAnimation;
};

abstract_class CNmReferencePoseNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	};
};

abstract_class CNmZeroPoseNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	};
};
#endif // ANIMGRAPHNODE_POSE_H
