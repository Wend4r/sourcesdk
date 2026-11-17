//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_VECTORS_H
#define ANIMGRAPHNODE_VECTORS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "tier0/platform.h"

abstract_class CNmVectorCreateNode : public CNmVectorValueNode
{
public:
	class CDefinition : public CNmVectorValueNode::CDefinition
	{
	public:
		int16 m_inputVectorValueNodeIdx;
		int16 m_inputValueXNodeIdx;
		int16 m_inputValueYNodeIdx;
		int16 m_inputValueZNodeIdx;
	};

	CNmVectorValueNode *m_pInputVectorValueNode;
	CNmFloatValueNode *m_pInputXValueNode;
	CNmFloatValueNode *m_pInputYValueNode;
	CNmFloatValueNode *m_pInputZValueNode;
	Vector m_value;
};

abstract_class CNmVectorInfoNode : public CNmFloatValueNode
{
public:
	enum class Info_t : uint8
	{
		X = 0,
		Y = 1,
		Z = 2,
		Length = 3,
		AngleHorizontal = 4,
		AngleVertical = 5,
	};

	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		CNmVectorInfoNode::Info_t m_desiredInfo;
	};

	CNmVectorValueNode *m_pInputValueNode;
	float m_value;
};

abstract_class CNmVectorNegateNode : public CNmVectorValueNode
{
public:
	class CDefinition : public CNmVectorValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
	};

	CNmVectorValueNode *m_pInputValueNode;
	Vector m_value;
};
#endif // ANIMGRAPHNODE_VECTORS_H
