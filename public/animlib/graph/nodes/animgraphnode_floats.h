//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_FLOATS_H
#define ANIMGRAPHNODE_FLOATS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraphnode.h"
#include "mathlib/piecewisecurve.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmFloatAngleMathNode : public CNmFloatValueNode
{
public:
	enum class Operation_t : uint8
	{
		ClampTo180 = 0,
		ClampTo360 = 1,
		FlipHemisphere = 2,
		FlipHemisphereNegate = 3,
	};

	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		CNmFloatAngleMathNode::Operation_t m_operation;
	};

	CNmFloatValueNode *m_pInputValueNode;
	float m_value;
};

abstract_class CNmFloatClampNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		Range_t m_clampRange;
	};

	CNmFloatValueNode *m_pInputValueNode;
	float m_value;
};

abstract_class CNmFloatComparisonNode : public CNmBoolValueNode
{
public:
	enum class Comparison_t : uint8
	{
		GreaterThanEqual = 0,
		LessThanEqual = 1,
		NearEqual = 2,
		GreaterThan = 3,
		LessThan = 4,
	};

	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		int16 m_nComparandValueNodeIdx;
		CNmFloatComparisonNode::Comparison_t m_comparison;
		float m_flEpsilon;
		float m_flComparisonValue;
	};

	CNmFloatValueNode *m_pInputValueNode;
	CNmFloatValueNode *m_pComparandValueNode;
	bool m_result;
};

abstract_class CNmFloatCurveNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		CPiecewiseCurve m_curve;
	};

	CNmFloatValueNode *m_pInputValueNode;
	float m_currentValue;
	CPiecewiseCurve m_curve;
};

abstract_class CNmFloatEaseNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		float m_flEaseTime;
		float m_flStartValue;
		int16 m_nInputValueNodeIdx;
		NmEasingOperation_t m_easingOp;
		bool m_bUseStartValue;
	};

	CNmFloatValueNode *m_pInputValueNode;
	Range_t m_easeRange;
	float m_currentValue;
	float m_currentEaseTime;
};

abstract_class CNmFloatMathNode : public CNmFloatValueNode
{
public:
	enum class Operator_t : uint8
	{
		Add = 0,
		Sub = 1,
		Mul = 2,
		Div = 3,
		Mod = 4,
		Abs = 5,
		Negate = 6,
		Floor = 7,
		Ceiling = 8,
		IntegerPart = 9,
		FractionalPart = 10,
		InverseFractionalPart = 11,
	};

	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdxA;
		int16 m_nInputValueNodeIdxB;
		bool m_bReturnAbsoluteResult;
		bool m_bReturnNegatedResult;
		CNmFloatMathNode::Operator_t m_operator;
		float m_flValueB;
	};

	CNmFloatValueNode *m_pValueNodeA;
	CNmFloatValueNode *m_pValueNodeB;
	float m_value;
};

abstract_class CNmFloatRangeComparisonNode : public CNmBoolValueNode
{
public:
	class CDefinition : public CNmBoolValueNode::CDefinition
	{
	public:
		Range_t m_range;
		int16 m_nInputValueNodeIdx;
		bool m_bIsInclusiveCheck;
	};

	CNmFloatValueNode *m_pInputValueNode;
	bool m_result;
};

abstract_class CNmFloatRemapNode : public CNmFloatValueNode
{
public:
	struct RemapRange_t
	{
		float m_flBegin;
		float m_flEnd;
	};

	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nInputValueNodeIdx;
		CNmFloatRemapNode::RemapRange_t m_inputRange;
		CNmFloatRemapNode::RemapRange_t m_outputRange;
	};

	CNmFloatValueNode *m_pInputValueNode;
	float m_value;
};

abstract_class CNmFloatSelectorNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		CUtlLeanVectorFixedGrowable< int16, 5 > m_conditionNodeIndices;
		CUtlLeanVectorFixedGrowable< float, 5 > m_values;
		float m_flDefaultValue;
		float m_flEaseTime;
		NmEasingOperation_t m_easingOp;
	};

	CUtlVectorFixedGrowable< CNmBoolValueNode *, 5 > m_conditionNodes;
	Range_t m_easeRange;
	float m_currentValue;
	float m_currentEaseTime;
};

abstract_class CNmFloatSpringNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		float m_flStartValue;
		float m_flHertz;
		float m_flDampingRatio;
		int16 m_nInputValueNodeIdx;
		bool m_bUseStartValue;
	};

	CNmFloatValueNode *m_pInputValueNode;
	float m_position;
	float m_velocity;
};

abstract_class CNmFloatSwitchNode : public CNmFloatValueNode
{
public:
	class CDefinition : public CNmFloatValueNode::CDefinition
	{
	public:
		int16 m_nSwitchValueNodeIdx;
		int16 m_nTrueValueNodeIdx;
		int16 m_nFalseValueNodeIdx;
		float m_flFalseValue;
		float m_flTrueValue;
	};

	CNmBoolValueNode *m_pSwitchValueNode;
	CNmFloatValueNode *m_pTrueValueNode;
	CNmFloatValueNode *m_pFalseValueNode;
	float m_value;
};
#endif // ANIMGRAPHNODE_FLOATS_H
