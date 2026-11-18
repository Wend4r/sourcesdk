//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_BLEND2D_H
#define ANIMGRAPHNODE_BLEND2D_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "mathlib/vector2d.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmBlend2DNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 5 > m_sourceNodeIndices;
		CUtlLeanVectorFixedGrowable< Vector2D, 10 > m_values;
		CUtlLeanVectorFixedGrowable< uint8, 30 > m_indices;
		CUtlLeanVectorFixedGrowable< uint8, 10 > m_hullIndices;
		int16 m_nInputParameterNodeIdx0;
		int16 m_nInputParameterNodeIdx1;
		bool m_bAllowLooping;
	};

	struct BlendSpaceResult_t
	{
		int32 m_sourceIndices[ 3 ];
		float m_blendWeightBetween0And1;
		float m_blendWeightBetween1And2;
		Vector2D m_finalParameter;
	};

	CUtlLeanVectorFixedGrowable< CNmPoseNode *, 5 > m_sourceNodes;
	CNmFloatValueNode *m_pInputParameterNode0;
	CNmFloatValueNode *m_pInputParameterNode1;
	CNmSyncTrack m_blendedSyncTrack;
	BlendSpaceResult_t m_bsr;
	uint32 m_blendSpaceUpdateID;
};
#endif // ANIMGRAPHNODE_BLEND2D_H
