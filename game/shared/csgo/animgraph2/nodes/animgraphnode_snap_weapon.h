//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes of Counter-Strike
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_SNAP_WEAPON_H
#define ANIMGRAPHNODE_SNAP_WEAPON_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/graph/nodes/animgraphnode_passthrough.h"
#include "tier0/platform.h"

// Only overrides the methods of its bases. The fields of the runtime node past
// CNmPassthroughNode are not known and not declared.
abstract_class CNmSnapWeaponNode : public CNmPassthroughNode
{
public:
	class CDefinition : public CNmPassthroughNode::CDefinition
	{
	public:
		int16 m_nFlashedAmountNodeIdx;
		int16 m_nWeaponCategoryNodeIdx;
		int16 m_nWeaponTypeNodeIdx;
	};
};
#endif // ANIMGRAPHNODE_SNAP_WEAPON_H
