//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_BLEND1D_H
#define ANIMGRAPHNODE_BLEND1D_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "mathlib/piecewisecurve.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmParameterizedBlendNode : public CNmPoseNode
{
public:
	struct BlendRange_t
	{
		int16 m_nInputIdx0;
		int16 m_nInputIdx1;
		Range_t m_parameterValueRange;
	};

	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 5 > m_sourceNodeIndices;
		int16 m_nInputParameterValueNodeIdx;
		bool m_bAllowLooping;
	};

	struct Parameterization_t
	{
		CUtlLeanVectorFixedGrowable< CNmParameterizedBlendNode::BlendRange_t, 5 > m_blendRanges;
		Range_t m_parameterRange;
	};

	struct BlendSpaceResult_t
	{
		CNmPoseNode *m_pSource0;
		CNmPoseNode *m_pSource1;
		float m_blendWeight;
		uint32 m_updateID;
	};

	CUtlVectorFixedGrowable< CNmPoseNode *, 5 > m_sourceNodes;
	CNmFloatValueNode *m_pInputParameterValueNode;
	BlendSpaceResult_t m_bsr;
	CNmSyncTrack m_blendedSyncTrack;
	Parameterization_t m_parameterization;
};

abstract_class CNmVelocityBlendNode : public CNmParameterizedBlendNode
{
public:
	class CDefinition : public CNmParameterizedBlendNode::CDefinition
	{
	};
};

abstract_class CNmBlend1DNode : public CNmParameterizedBlendNode
{
public:
	class CDefinition : public CNmParameterizedBlendNode::CDefinition
	{
	public:
		CNmParameterizedBlendNode::Parameterization_t m_parameterization;
	};
};
#endif // ANIMGRAPHNODE_BLEND1D_H
