//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_STATE_H
#define ANIMGRAPHNODE_STATE_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmStateNode : public CNmPoseNode
{
public:
	struct TimedEvent_t
	{
		enum class Comparison_t : uint8
		{
			LessThanEqual = 0,
			GreaterThanEqual = 1,
		};

		CGlobalSymbol m_ID;
		float m_flTimeValueSeconds;
		CNmStateNode::TimedEvent_t::Comparison_t m_comparisionOperator;
	};

	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nChildNodeIdx;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 3 > m_entryEvents;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 3 > m_executeEvents;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 3 > m_exitEvents;
		CUtlLeanVectorFixedGrowable< CNmStateNode::TimedEvent_t, 1 > m_timedRemainingEvents;
		CUtlLeanVectorFixedGrowable< CNmStateNode::TimedEvent_t, 1 > m_timedElapsedEvents;
		int16 m_nLayerWeightNodeIdx;
		int16 m_nLayerRootMotionWeightNodeIdx;
		int16 m_nLayerBoneMaskNodeIdx;
		bool m_bIsOffState;
		bool m_bUseActualElapsedTimeInStateForTimedEvents;
	};

	enum class TransitionState_t : uint8
	{
		None = 0,
		TransitioningIn,
		TransitioningOut,
	};

	// Sets the bit of the state in the transitioning flags of its state machine, which holds up to 128 states
	struct TransitionFlag_t
	{
		uint64 *m_pFlags[ 2 ];
		uint64 m_flagMask[ 2 ];
	};

	CNmPoseNode *m_pChildNode;
	NmSampledEventRange_t m_sampledEventRange;
	CNmBoneMaskValueNode *m_pBoneMaskNode;
	CNmFloatValueNode *m_pLayerWeightNode;
	CNmFloatValueNode *m_pLayerRootMotionWeightNode;
	TransitionState_t m_transitionState;
	TransitionFlag_t m_transitionFlag;
	float m_elapsedTime;
	bool m_isFirstStateUpdate;
};
#endif // ANIMGRAPHNODE_STATE_H
