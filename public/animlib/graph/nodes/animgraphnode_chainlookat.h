//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_CHAINLOOKAT_H
#define ANIMGRAPHNODE_CHAINLOOKAT_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animblendweight.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "mathlib/vector.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmChainLookatNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		CGlobalSymbol m_endEffectorBoneID;
		Vector m_endEffectorForwardAxis;
		Vector m_endEffectorOffset;
		int16 m_nLookatTargetNodeIdx;
		int16 m_nEnabledNodeIdx;
		float m_flBlendTimeSeconds;
		CUtlVectorFixedGrowable< float, 5 > m_chainWeights;
		uint8 m_nChainLength;
		bool m_bIsTargetInWorldSpace;
	};

	// What the node hands to CNmChainLookatTask
	struct TaskParams_t
	{
		int32 m_effectorBoneIdx;

		// Number of bones found above the effector, at most m_nChainLength of the definition
		int32 m_chainLength;
		Vector m_endEffectorForwardAxis;
		Vector m_endEffectorOffset;
		CUtlVectorFixedGrowable< float, 5 > m_chainWeights;
		float m_blendWeight;
		Vector m_lookatTarget;
		bool m_isTargetInWorldSpace;
	};

	CNmTargetValueNode *m_pLookatTargetNode;
	CNmBoolValueNode *m_pEnabledNode;
	NmBlendWeight_t m_blendWeight;
	TaskParams_t m_taskParams;
	Vector m_lastLookatTarget;
	bool m_isValidSetup;
};
#endif // ANIMGRAPHNODE_CHAINLOOKAT_H
