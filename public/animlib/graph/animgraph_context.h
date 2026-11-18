//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: AnimGraph2 graph context: the per-update state a graph instance
//          hands to its nodes
//
//===========================================================================//

#ifndef ANIMGRAPH_CONTEXT_H
#define ANIMGRAPH_CONTEXT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlleanvector.h"
#include "mathlib/transform.h"

class CNmSkeleton;
class CNmTaskSystem;
class CNmSampledEventsBuffer;
class CNmRootMotionDebugger;
class CNmGraphTimeInfo;
struct NmGraphLayerContext_t;
struct NmGraphLogEntry_t;

// Whether a node is evaluated for a state the graph is in or for one it is transitioning away from
enum class NmBranchState_t : int32
{
	Active = 0,
	Inactive,
};

// Chain of node indices that leads from the root graph to the graph instance being evaluated
struct NmSourcePath_t
{
	CUtlLeanVectorFixedGrowable< int16, 8 > m_path;
	// Recomputed whenever the path changes
	uint64 m_nHash;
};

class CNmGraphContext
{
public:
	// The ID the creator of the graph instance passed in
	int32 m_graphUserID;
	const CNmSkeleton *m_pSkeleton;

	// Only set while the graph state is being reset
	const CNmGraphTimeInfo *m_pInitializationTimeInfo;

	// Incremented by every evaluation; a node whose m_lastUpdateID matches was updated this frame
	uint32 m_updateID;
	NmBranchState_t m_branchState;
	CTransform m_worldTransform;
	CTransform m_worldTransformInverse;
	// Set by a layer node while it evaluates its layers
	NmGraphLayerContext_t *m_pLayerContext;
	float m_deltaTime;
	// IApplication::IsInToolsMode() at the time the instance was created; enables m_pLog and m_pRootMotionDebugger
	bool m_bInToolsMode;
	// Only set while the instance draws its debug visualization
	const CUtlVector< NmSourcePath_t > *m_pNodesToDebug;

	CUtlVector< int16 > m_activeNodes;
	CUtlVector< NmGraphLogEntry_t > *m_pLog;
	int32 m_lastOutputtedLogItemIdx;
	NmSourcePath_t m_basePath;
	// False for the context of a referenced graph, which shares the pointers below with its parent
	bool m_isStandaloneGraphContext;
	bool m_isInitialized;

	CNmTaskSystem *m_pTaskSystem;
	CNmSampledEventsBuffer *m_pSampledEventsBuffer;
	CNmRootMotionDebugger *m_pRootMotionDebugger;
};

#endif // ANIMGRAPH_CONTEXT_H
