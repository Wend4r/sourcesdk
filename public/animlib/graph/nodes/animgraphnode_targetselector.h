//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_TARGETSELECTOR_H
#define ANIMGRAPHNODE_TARGETSELECTOR_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_animationclip.h"
#include "mathlib/transform.h"
#include "mathlib/vector.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmTargetSelectorNode : public CNmClipReferenceNode
{
public:
	class CDefinition : public CNmClipReferenceNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 8 > m_optionNodeIndices;
		float m_flOrientationScoreWeight;
		float m_flPositionScoreWeight;
		int16 m_parameterNodeIdx;
		bool m_bIgnoreInvalidOptions;
		bool m_bIsWorldSpaceTarget;
		CGlobalSymbol m_alignmentBoneID;
	};

	struct Option_t
	{
		int32 m_optionIdx;
		Vector m_endPoint;
		QuaternionAligned m_endOrientation;
		float m_orientationScore;
		float m_positionScore;
	};

	CUtlLeanVectorFixedGrowable< CNmClipReferenceNode *, 5 > m_optionNodes;
	CNmTargetValueNode *m_pParameterNode;
	CNmClipReferenceNode *m_pSelectedNode;
	CTransform m_targetTransform;
	CUtlVectorFixedGrowable< Option_t, 10 > m_selectionOptions;
	int32 m_selectedOptionIdx;
	CTransform m_selectionWorldTransform;
	int32 m_alignmentBoneIdx;
};
#endif // ANIMGRAPHNODE_TARGETSELECTOR_H
