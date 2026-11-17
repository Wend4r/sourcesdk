//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_SPEEDSCALE_H
#define ANIMGRAPHNODE_SPEEDSCALE_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "tier0/platform.h"

class CNmGraphContext;

abstract_class CNmSpeedScaleBaseNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		float m_flDefaultInputValue;
	};

	virtual float CalculateSpeedScaleMultiplier( CNmGraphContext &context ) const = 0;
};

abstract_class CNmSpeedScaleNode : public CNmSpeedScaleBaseNode
{
public:
	class CDefinition : public CNmSpeedScaleBaseNode::CDefinition
	{
	};

	CNmFloatValueNode *m_pScaleValueNode;
};

abstract_class CNmVelocityBasedSpeedScaleNode : public CNmSpeedScaleBaseNode
{
public:
	class CDefinition : public CNmSpeedScaleBaseNode::CDefinition
	{
	};

	CNmFloatValueNode *m_pDesiredVelocityValueNode;
};

abstract_class CNmDurationScaleNode : public CNmSpeedScaleBaseNode
{
public:
	class CDefinition : public CNmSpeedScaleBaseNode::CDefinition
	{
	};

	CNmFloatValueNode *m_pDurationValueNode;
};
#endif // ANIMGRAPHNODE_SPEEDSCALE_H
