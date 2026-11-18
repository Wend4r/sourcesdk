//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_PARAMETERS_H
#define ANIMGRAPHNODE_PARAMETERS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "animlib/tasksystem/animbonemasktask.h"
#include "tier0/platform.h"

abstract_class CNmControlParameterBoolNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	};

	bool m_value;
};

abstract_class CNmControlParameterFloatNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	};

	float m_value;
};

abstract_class CNmControlParameterIDNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	};

	CGlobalSymbol m_value;
};

abstract_class CNmControlParameterTargetNode : public CNmTargetValueNode
{
public:
	class CDefinition : public CNmTargetValueNode::CDefinition
	{
	};

	CNmTarget m_value;
};

abstract_class CNmControlParameterVectorNode : public CNmVectorValueNode
{
public:
	class CDefinition : public CNmVectorValueNode::CDefinition
	{
	};

	Vector m_value;
};

abstract_class CNmVirtualParameterBoneMaskNode : public CNmBoneMaskValueNode
{
public:
	class CDefinition : public CNmBoneMaskValueNode::CDefinition
	{
	public:
		int16 m_nChildNodeIdx;
	};

	CNmBoneMaskValueNode *m_pChildNode;
	CNmBoneMaskTaskList m_value;
};

abstract_class CNmVirtualParameterBoolNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nChildNodeIdx;
	};

	CNmBoolValueNode *m_pChildNode;
	bool m_value;
};

abstract_class CNmVirtualParameterFloatNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nChildNodeIdx;
	};

	CNmFloatValueNode *m_pChildNode;
	float m_value;
};

abstract_class CNmVirtualParameterIDNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	public:
		int16 m_nChildNodeIdx;
	};

	CNmIDValueNode *m_pChildNode;
	CGlobalSymbol m_value;
};

abstract_class CNmVirtualParameterTargetNode : public CNmTargetValueNode
{
public:
	class CDefinition : public CNmTargetValueNode::CDefinition
	{
	public:
		int16 m_nChildNodeIdx;
	};

	CNmTargetValueNode *m_pChildNode;
	CNmTarget m_value;
};

abstract_class CNmVirtualParameterVectorNode : public CNmVectorValueNode
{
public:
	class CDefinition : public CNmVectorValueNode::CDefinition
	{
	public:
		int16 m_nChildNodeIdx;
	};

	CNmVectorValueNode *m_pChildNode;
	Vector m_value;
};
#endif // ANIMGRAPHNODE_PARAMETERS_H
