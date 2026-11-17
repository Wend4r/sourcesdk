//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_TARGETS_H
#define ANIMGRAPHNODE_TARGETS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "mathlib/vector.h"
#include "tier0/platform.h"

abstract_class CNmIsTargetSetNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
	};

	CNmTargetValueNode *m_pInputValueNode;
	bool m_result;
};

abstract_class CNmTargetInfoNode : public CNmFloatValueNode
{
public:
	enum class Info_t : int32
	{
		AngleHorizontal = 0,
		AngleVertical = 1,
		Distance = 2,
		DistanceHorizontalOnly = 3,
		DistanceVerticalOnly = 4,
		DeltaOrientationX = 5,
		DeltaOrientationY = 6,
		DeltaOrientationZ = 7,
	};

	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		CNmTargetInfoNode::Info_t m_infoType;
		bool m_bIsWorldSpaceTarget;
	};

	CNmTargetValueNode *m_pTargetNode;
	float m_value;
};

abstract_class CNmTargetOffsetNode : public CNmTargetValueNode
{
public:
	class CDefinition : public CNmTargetValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		bool m_bIsBoneSpaceOffset;
		QuaternionAligned m_rotationOffset;
		Vector m_translationOffset;
	};

	CNmTargetValueNode *m_pInputValueNode;
	CNmTarget m_value;
};

abstract_class CNmTargetPointNode : public CNmVectorValueNode
{
public:
	class CDefinition : public CNmVectorValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		bool m_bIsWorldSpaceTarget;
	};

	CNmTargetValueNode *m_pTargetNode;
	Vector m_value;
};
#endif // ANIMGRAPHNODE_TARGETS_H
