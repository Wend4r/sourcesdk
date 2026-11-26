//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: State of a root motion warp: the root motion of a clip bent so
//          that it ends at a target transform
//
//===========================================================================//

#ifndef ANIMROOTMOTIONWARP_H
#define ANIMROOTMOTIONWARP_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "tier0/utlvector.h"
#include "mathlib/transform.h"
#include "animlib/animlib_types.h"

class CNmRootMotionWarpContext;

// Position in a clip: a frame and the progress towards the next one
struct NmFrameTime_t
{
	int32 m_frameIndex;
	NmPercent_t m_percentageThrough;
};

// The only implementation in the game is CNmRootMotionWarpDebugData
abstract_class INmRootMotionWarpDebugData
{
public:
	virtual ~INmRootMotionWarpDebugData() = 0;

	// Named by what it does: draws the debug state through the draw context, at the world transform
	virtual void DrawDebug( const CTransform &worldTransform, void *pDrawContext ) const = 0;
};

class CNmRootMotionWarpContext
{
public:
	// Frame range covered by one warp event of the clip
	struct Section_t
	{
		int32 m_startFrame;
		int32 m_endFrame;
		NmTargetWarpRule_t m_warpRule;
		NmTargetWarpAlgorithm_t m_translationAlgorithm;
	};

	enum class LogSeverity_t : int32
	{
		Info = 0,
		Warning,
		Error,
		FatalError,
	};

	struct LogEntry_t
	{
		LogSeverity_t m_severity;
		CBufferStringN< 256 > m_message;
	};

	bool HasWarpedRootMotion() const { return m_warpedRootMotion.m_nNumFrames > 0; }

	NmFrameTime_t m_startTime;
	CTransform m_startTransform;
	CTransform m_targetTransform;
	CNmRootMotionData m_rootMotion;
	CNmRootMotionData m_warpedRootMotion;
	CUtlVector< Section_t > m_sections;
	CUtlVector< CTransform > m_deltaTransforms;
	CUtlVector< CTransform > m_inverseDeltaTransforms;

	// Filled only while debugging is enabled
	CUtlVector< LogEntry_t > m_log;
	bool m_isDebuggingEnabled;
	bool m_alignTargetAtLastSection;
	INmRootMotionWarpDebugData *m_pDebugData;
};

#endif // ANIMROOTMOTIONWARP_H
