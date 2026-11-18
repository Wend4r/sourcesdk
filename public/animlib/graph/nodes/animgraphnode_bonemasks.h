//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_BONEMASKS_H
#define ANIMGRAPHNODE_BONEMASKS_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraphnode.h"
#include "animlib/tasksystem/animbonemasktask.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmBoneMaskBlendNode : public CNmBoneMaskValueNode
{
public:
	class CDefinition : public CNmBoneMaskValueNode::CDefinition
	{
	public:
		int16 m_nSourceMaskNodeIdx;
		int16 m_nTargetMaskNodeIdx;
		int16 m_nBlendWeightValueNodeIdx;
	};

	CNmBoneMaskValueNode *m_pSourceBoneMask;
	CNmBoneMaskValueNode *m_pTargetBoneMask;
	CNmFloatValueNode *m_pBlendWeightValueNode;
	CNmBoneMaskTaskList m_taskList;
};

abstract_class CNmBoneMaskNode : public CNmBoneMaskValueNode
{
public:
	class CDefinition : public CNmBoneMaskValueNode::CDefinition
	{
	public:
		CGlobalSymbol m_boneMaskID;
	};

	CNmBoneMaskTaskList m_taskList;
};

abstract_class CNmBoneMaskSelectorNode : public CNmBoneMaskValueNode
{
public:
	class CDefinition : public CNmBoneMaskValueNode::CDefinition
	{
	public:
		int16 m_defaultMaskNodeIdx;
		int16 m_parameterValueNodeIdx;
		bool m_bSwitchDynamically;
		CUtlLeanVectorFixedGrowable< int16, 8 > m_maskNodeIndices;
		CUtlLeanVectorFixedGrowable< CGlobalSymbol, 7 > m_parameterValues;
		float m_flBlendTimeSeconds;
	};

	CNmIDValueNode *m_pParameterValueNode;
	CNmBoneMaskValueNode *m_pDefaultMaskValueNode;
	CUtlVectorFixedGrowable< CNmBoneMaskValueNode *, 7 > m_boneMaskOptionNodes;
	int32 m_selectedMaskIndex;
	int32 m_newMaskIndex;
	CNmBoneMaskTaskList m_taskList;
	float m_currentTimeInBlend;
	bool m_isBlending;
};

abstract_class CNmBoneMaskSwitchNode : public CNmBoneMaskValueNode
{
public:
	class CDefinition : public CNmBoneMaskValueNode::CDefinition
	{
	public:
		int16 m_nSwitchValueNodeIdx;
		int16 m_nTrueValueNodeIdx;
		int16 m_nFalseValueNodeIdx;
		float m_flBlendTimeSeconds;
		bool m_bSwitchDynamically;
	};

	CNmBoolValueNode *m_pSwitchValueNode;
	CNmBoneMaskValueNode *m_pTrueMaskValueNode;
	CNmBoneMaskValueNode *m_pFalseMaskValueNode;
	uint64 m_nUnknown30;
	CNmBoneMaskTaskList m_taskList;
	float m_currentTimeInBlend;
	int32 m_selectedMaskIndex;
	bool m_isBlending;
};

abstract_class CNmFixedWeightBoneMaskNode : public CNmBoneMaskValueNode
{
public:
	class CDefinition : public CNmBoneMaskValueNode::CDefinition
	{
	public:
		float m_flBoneWeight;
	};

	CNmBoneMaskTaskList m_taskList;
};
#endif // ANIMGRAPHNODE_BONEMASKS_H
