//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_ANIMATIONCLIP_H
#define ANIMGRAPHNODE_ANIMATIONCLIP_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/animlib_types.h"
#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "resourcesystem/resourcehandle.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

class CNmClip;

abstract_class CNmClipReferenceNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	};

	virtual bool HasAnimation() const = 0;
	virtual const CNmClip *GetAnimation() const = 0;
	virtual void DisableRootMotionSampling() = 0;
	virtual bool IsLooping() const = 0;
};

abstract_class CNmClipNode : public CNmClipReferenceNode
{
public:
	class CDefinition : public CNmClipReferenceNode::CDefinition
	{
	public:
		int16 m_nPlayInReverseValueNodeIdx;
		bool m_bSampleRootMotion;
		bool m_bAllowLooping;
		int16 m_nDataSlotIdx;
		int16 m_nResetTimeValueNodeIdx;
		CUtlVectorFixedGrowable< CGlobalSymbol, 2 > m_graphEvents;
		float m_flSpeedMultiplier;
		int32 m_nStartSyncEventOffset;
	};

	CWeakHandle< InfoForResourceTypeCNmClip > m_pAnimation;
	CNmBoolValueNode *m_pPlayInReverseValueNode;
	CNmBoolValueNode *m_pResetTimeValueNode;
	CNmSyncTrack *m_pSyncTrack;
	bool m_shouldPlayInReverse;
	bool m_shouldSampleRootMotion;
	bool m_isFirstUpdate;
	bool m_hasLooped;
};
#endif // ANIMGRAPHNODE_ANIMATIONCLIP_H
