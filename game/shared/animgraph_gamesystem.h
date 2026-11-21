//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that ticks the animation graphs of the entities on
//			its update list (CS2 game/shared/animgraph_gamesystem.cpp). Factory
//			"AnimGraphGameSystem".
//
//=============================================================================//

#ifndef ANIMGRAPH_GAMESYSTEM_H
#define ANIMGRAPH_GAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "resourcesystem/itoolsresourcelistener.h"
#include "tier0/threadtools.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlsymbol.h"
#include "tier0/utlvector.h"

class CJob;

//-----------------------------------------------------------------------------
// Reallocating factory: 65960 bytes on Windows, 66056 on Linux (the two read-write locks at
// the end are std::shared_mutex, 8 bytes on Windows and 56 on Linux). No source has the
// class; the members are named by what the code does, the profiler scopes and messages of
// the .cpp ("AnimTickUpdate", "OnServerPrePackEntities", "UpdateAnimGraph2Animations",
// "AddedToUpdateList", the "Pose Evaluation Report" of anim_eval_stats).
// libserver.so repeats the overrides of the two listeners at the end of the main table
// (slots 66 and 67).
//-----------------------------------------------------------------------------
class CAnimGraphGameSystem : public IGameSystem, public IToolsResourceListener, public IToolsResourcePreReloadListener
{
public:
	DECLARE_GAME_SYSTEM();

	virtual ~CAnimGraphGameSystem();

	// Registers the two listeners with the resource system, clears m_UpdateListIndices and,
	// when the animation graph resource manager exists, fills m_LoadedGraphResources
	GS_EVENT( GameInit );
	// Unregisters the listeners and empties the update list, the hash tables and
	// m_LoadedGraphResources
	GS_EVENT( GameShutdown );
	// In tools mode tells every animating entity to apply pending graph changes (same body as
	// ServerPreEntityThink)
	GS_EVENT( ClientPreUpdate );
	// AnimTickUpdate for the client ("AnimGraph Client Tick") on the first tick of the frame
	GS_EVENT( ClientPostUpdate );
	// In tools mode tells every animating entity to apply pending graph changes
	GS_EVENT( ServerPreEntityThink );
	// AnimTickUpdate for the server ("AnimGraph Server Tick")
	GS_EVENT( ServerPostEntityThink );
	// "OnServerPrePackEntities": finishes the graphs of the update list that still have
	// work, in parallel jobs
	GS_EVENT( ServerPrePackEntities );

	// Forward to the game system factory
	virtual bool DoesGameSystemReallocate() override;
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;

	// IToolsResourceListener: for every reloaded skeleton, graph or clip resource (vnmskel,
	// vnmgraph, vnmclip) tells every animating entity
	virtual void NotifyResourceStatusChange( const ResourceStatusChange_t *pChanges, int64 nCount ) override;
	// IToolsResourcePreReloadListener: the same for a skeleton, graph or clip about to be
	// reloaded
	virtual void NotifyResourcePreReload( ResourceHandle_t hResource, ResourceType_t nResourceType ) override;

public:
	// The counters of one animation graph class in the pose evaluation report, 12 bytes
	struct PoseEvaluationStats_t
	{
		// "EvaluatePoses Used" denominator: ticks the class evaluated a pose
		int m_nEvaluations;
		// Evaluated poses whose bones were used
		int m_nUsed;
		// Bones used without an evaluated pose
		int m_nUsedWithoutEvaluation;
	};

	// An entity of the update list, 8 bytes
	struct UpdateListEntry_t
	{
		CEntityHandle m_hEntity;
		// Zero when added; bit 4 makes the update skip a graph that still has a pending
		// evaluation
		uint16 m_nFlags;
	};

	// anim_eval_stats: the game time until which the pose evaluations are counted (0 when off)
	float m_flEvalStatsEndTime;
	// Ticks counted for the pose evaluation report
	int m_nEvalStatsTicks;
	// The pose evaluations of this tick per entity: bit 1 evaluated, bit 2 used
	CUtlHashtable< CEntityHandle, uint8 > m_PoseEvaluations;
	CThreadFastMutex m_PoseEvaluationsLock;
	// Indexed by the symbol of the graph class name in m_PoseEvaluationStatsNames
	CUtlVector< PoseEvaluationStats_t > m_PoseEvaluationStats;
	CUtlSymbolTable m_PoseEvaluationStatsNames;
	// The totals of the pose evaluation report
	int m_nTotalPoseEvaluations;
	int m_nTotalPoseEvaluationsUsed;
	int m_nTotalUsedWithoutEvaluation;
	// Seconds the last AnimTickUpdate took
	float m_flLastTickUpdateDuration;
	// Position of each entity (by entity index) in m_UpdateList, 0xFFFF when it is not on it
	uint16 m_UpdateListIndices[ 32768 ];
	// The entities whose graphs are ticked ("AddedToUpdateList")
	CUtlVector< UpdateListEntry_t > m_UpdateList;
	// The next index of m_UpdateList a worker takes in the parallel loops
	CInterlockedInt m_nNextUpdateIndex;
	// The "UpdateAnimGraph2Animations" jobs of the current tick (a reference is held)
	CUtlVector< CJob * > m_UpdateJobs;
	// GameInit's list of the loaded animation graph resources (objects of the resource
	// system it hands to the graph resource manager)
	CUtlVector< void * > m_LoadedGraphResources;
	// The three hash tables below are only constructed, emptied (GameShutdown) and destroyed
	// (on the server and in the client): 40-byte entries of an
	// 8-byte key and a CUtlVector. The key and element types are not known; the first table
	// is another template instance than the other two
	CUtlHashtable< uint64, CUtlVector< uint64 > > m_UnusedTable1;
	CUtlHashtable< void *, CUtlVector< void * > > m_UnusedTable2;
	CUtlHashtable< void *, CUtlVector< void * > > m_UnusedTable3;
	// Only the constructor touches these two locks
	CThreadSpinRWLock m_UnusedLock1;
	CThreadSpinRWLock m_UnusedLock2;
};

#endif // ANIMGRAPH_GAMESYSTEM_H
