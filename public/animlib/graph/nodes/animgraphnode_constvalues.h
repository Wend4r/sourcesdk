//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_CONSTVALUES_H
#define ANIMGRAPHNODE_CONSTVALUES_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "mathlib/vector.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"

abstract_class CNmConstBoolNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		bool m_bValue;
	};
};

abstract_class CNmConstFloatNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		float m_flValue;
	};
};

abstract_class CNmConstIDNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	public:
		CGlobalSymbol m_value;
	};
};

abstract_class CNmConstTargetNode : public CNmTargetValueNode
{
public:
	class CDefinition : public CNmTargetValueNode::CDefinition
	{
	public:
		CNmTarget m_value;
	};
};

abstract_class CNmConstVectorNode : public CNmVectorValueNode
{
public:
	class CDefinition : public CNmVectorValueNode::CDefinition
	{
	public:
		Vector m_value;
	};
};
#endif // ANIMGRAPHNODE_CONSTVALUES_H
