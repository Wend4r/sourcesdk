//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_LAYERS_H
#define ANIMGRAPHNODE_LAYERS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmLayerBlendNode : public CNmPoseNode
{
public:
	struct LayerDefinition_t
	{
		int16 m_nInputNodeIdx;
		int16 m_nWeightValueNodeIdx;
		int16 m_nBoneMaskValueNodeIdx;
		int16 m_nRootMotionWeightValueNodeIdx;
		bool m_bIsSynchronized;
		bool m_bIgnoreEvents;
		bool m_bIsStateMachineLayer;
		NmPoseBlendMode_t m_blendMode;
	};

	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nBaseNodeIdx;
		bool m_bOnlySampleBaseRootMotion;
		CUtlLeanVectorFixedGrowable< CNmLayerBlendNode::LayerDefinition_t, 3 > m_layerDefinition;
	};

	struct Layer_t
	{
		CNmPoseNode *m_pInputNode;
		CNmFloatValueNode *m_pWeightValueNode;
		CNmFloatValueNode *m_pRootMotionWeightValueNode;
		CNmBoneMaskValueNode *m_pBoneMaskValueNode;
		float m_weight;
	};

	CNmPoseNode *m_pBaseLayerNode;
	CUtlLeanVectorFixedGrowable< Layer_t, 3 > m_layers;
	int16 m_rootMotionActionIdxBase;
};
#endif // ANIMGRAPHNODE_LAYERS_H
