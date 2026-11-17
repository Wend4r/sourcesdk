//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph nodes
//
//===========================================================================//

#ifndef ANIMGRAPHNODE_TIMECONTROLLEDCLIP_H
#define ANIMGRAPHNODE_TIMECONTROLLEDCLIP_H
#ifdef _WIN32
#pragma once
#endif

#include "animlib/graph/animgraph_valuetypes.h"
#include "animlib/graph/animgraphnode.h"
#include "resourcesystem/resourcehandle.h"
#include "tier0/globalsymbol.h"
#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"

abstract_class CNmTimeControlledClipNode : public CNmPoseNode
{
public:
	class CDefinition : public CNmPoseNode::CDefinition
	{
	public:
		int16 m_nPlayInReverseValueNodeIdx;
		bool m_bSampleRootMotion;
		int16 m_nDataSlotIdx;
		int16 m_nTimeValueNodeIdx;
		CUtlVectorFixedGrowable< CGlobalSymbol, 2 > m_graphEvents;
	};

	CWeakHandle< InfoForResourceTypeCNmClip > m_pAnimation;
	CNmFloatValueNode *m_pTimeValueNode;
	CNmBoolValueNode *m_pPlayInReverseValueNode;
	bool m_shouldPlayInReverse;
	bool m_isFirstUpdate;
	bool m_hasLooped;
};
#endif // ANIMGRAPHNODE_TIMECONTROLLEDCLIP_H
