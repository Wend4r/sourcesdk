//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_CACHEDVALUES_H
#define ANIMGRAPHNODE_CACHEDVALUES_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"

abstract_class CNmCachedBoolNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		NmCachedValueMode_t m_mode;
	};

	CNmBoolValueNode *m_pInputValueNode;
	bool m_value;
	bool m_hasCachedValue;
};

abstract_class CNmCachedFloatNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		NmCachedValueMode_t m_mode;
	};

	CNmFloatValueNode *m_pInputValueNode;
	float m_value;
	bool m_hasCachedValue;
};

abstract_class CNmCachedIDNode : public CNmIDValueNode
{
public:
	class CDefinition : public CNmIDValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		NmCachedValueMode_t m_mode;
	};

	CNmIDValueNode *m_pInputValueNode;
	CGlobalSymbol m_value;
	bool m_hasCachedValue;
};

abstract_class CNmCachedTargetNode : public CNmTargetValueNode
{
public:
	class CDefinition : public CNmTargetValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		NmCachedValueMode_t m_mode;
	};

	CNmTargetValueNode *m_pInputValueNode;
	CNmTarget m_value;
	bool m_hasCachedValue;
};

abstract_class CNmCachedVectorNode : public CNmVectorValueNode
{
public:
	class CDefinition : public CNmVectorValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		NmCachedValueMode_t m_mode;
	};

	CNmVectorValueNode *m_pInputValueNode;
	Vector m_value;
	bool m_hasCachedValue;
};
#endif // ANIMGRAPHNODE_CACHEDVALUES_H
