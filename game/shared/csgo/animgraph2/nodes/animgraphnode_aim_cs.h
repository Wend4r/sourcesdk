//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes of Counter-Strike
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_AIM_CS_H
#define ANIMGRAPHNODE_AIM_CS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "tier0/platform.h"

// Only overrides the methods of its bases. The fields of the runtime node past
// CNmPassthroughNode are not known and not declared.
abstract_class CNmAimCSNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		int16 m_nVerticalAngleNodeIdx;
		int16 m_nHorizontalAngleNodeIdx;
		int16 m_nWeaponCategoryNodeIdx;
		int16 m_nWeaponTypeNodeIdx;
		int16 m_nWeaponActionNodeIdx;
		int16 m_nWeaponDropNodeIdx;
		int16 m_nIsDefusingNodeIdx;
		int16 m_nCrouchWeightNodeIdx;
		float m_flHandIKBlendInTimeSeconds;
		float m_flActionBlendTimeSeconds;
		float m_flPlantingBlendTimeSeconds;
		float m_flTransitionBlendTimeSeconds;
	};
};
#endif // ANIMGRAPHNODE_AIM_CS_H
