//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_ISEXTERNALPOSESET_H
#define ANIMGRAPHNODE_ISEXTERNALPOSESET_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"

class CNmExternalPoseNode;

abstract_class CNmIsExternalPoseSetNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nExternalPoseNodeIdx;
	};

	CNmExternalPoseNode *m_pExternalPoseNode;
	bool m_result;
};
#endif // ANIMGRAPHNODE_ISEXTERNALPOSESET_H
